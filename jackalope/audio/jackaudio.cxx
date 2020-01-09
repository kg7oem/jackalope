// Jackalope Audio Engine
// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/audio/jackaudio.h>
#include <jackalope/pcm.h>
#include <jackalope/library.h>
#include <jackalope/logging.h>
#include <jackalope/string.h>

namespace jackalope {

namespace audio {

static shared_t<jackaudio_node_t> jackaudio_node_constructor(const init_args_t init_args_in)
{
    return jackalope::make_shared<jackaudio_node_t>(init_args_in);
}

static shared_t<jackaudio_connection_daemon_t> jackaudio_connections_constructor(const string_t& type_in, const init_args_t init_args_in)
{
    return jackalope::make_shared<jackaudio_connection_daemon_t>(type_in, init_args_in);
}

void jackaudio_init()
{
    add_object_constructor(JACKALOPE_AUDIO_JACKAUDIO_OBJECT_TYPE, jackaudio_node_constructor);
    add_daemon_constructor(JACKALOPE_AUDIO_JACKAUDIO_DAEMON_TYPE, jackaudio_connections_constructor);
}

jackaudio_node_t::jackaudio_node_t(const init_args_t init_args_in)
: node_t(init_args_in)
{ }

jackaudio_node_t::~jackaudio_node_t()
{
    if (jack_client != nullptr) {
        jack_deactivate(jack_client);
        jack_client_close(jack_client);

        jack_client = nullptr;
    }
}

shared_t<source_t> jackaudio_node_t::add_source(const string_t& source_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sinks_by_name.count(source_name_in) != 0) {
        throw_runtime_error("jackaudio sources can not have same name as sinks: ", source_name_in);
    }

    return node_t::add_source(source_name_in, type_in);
}

shared_t<sink_t> jackaudio_node_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sources_by_name.count(sink_name_in) != 0) {
        throw_runtime_error("jackaudio sinks can not have same name as sources: ", sink_name_in);
    }

    return node_t::add_sink(sink_name_in, type_in);
}

void jackaudio_node_t::init()
{
    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_AUDIO_JACKAUDIO_PROPERTY_CONFIG_CLIENT_NAME, property_t::type_t::string, init_args);

    return node_t::init();
}

void jackaudio_node_t::activate()
{
    assert_lockable_owner();

    for (auto i : { JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, JACKALOPE_PROPERTY_PCM_BUFFER_SIZE }) {
        set_undef_property(i);
    }

    for (auto& i : init_args_find("source", init_args)) {
        auto source_name = split_string(i.first, '.').at(1);
        auto source_type = i.second;
        add_source(source_name, source_type);
    }

    for (auto& i : init_args_find("sink", init_args)) {
        auto sink_name = split_string(i.first, '.').at(1);
        auto sink_type = i.second;
        add_sink(sink_name, sink_type);
    }

    node_t::activate();

    open_client();

    auto jack_sample_rate = jack_get_sample_rate(jack_client);
    auto sample_rate_property = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);
    auto jack_buffer_size = jack_get_buffer_size(jack_client);
    auto buffer_size_property = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE);

    if (! sample_rate_property->is_defined()) {
        sample_rate_property->set(jack_sample_rate);
    }

    if (! buffer_size_property->is_defined()) {
        buffer_size_property->set(jack_buffer_size);
    }

    for(auto& i : sources) {
        add_port(i->name, JACK_DEFAULT_AUDIO_TYPE, jackaudio::JackPortIsInput);
    }

    for(auto& i : sinks) {
        add_port(i->name, JACK_DEFAULT_AUDIO_TYPE, jackaudio::JackPortIsOutput);
    }

    auto helper = [] (const jackaudio_nframes_t num_frames_in, void * user_data) -> int_t {
        auto us = static_cast<jackaudio_node_t *>(user_data);
        auto shared_this = us->shared_obj<jackaudio_node_t>();
        return shared_this->handle_jack_process(num_frames_in);
    };

    if (jack_set_process_callback(jack_client, helper, static_cast<void *>(this))) {
        throw_runtime_error("could not set jackaudio audio processing callback");
    }

    if (jack_activate(jack_client)) {
        throw_runtime_error("Could not activate jackaudio client with server");
    }
}

void jackaudio_node_t::start()
{
    assert_lockable_owner();

    node_t::start();
}

bool jackaudio_node_t::should_run()
{
    assert_lockable_owner();

    if (thread_run_flag) {
        return false;
    }

    for (auto i : sinks) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

void jackaudio_node_t::run()
{
    assert_lockable_owner();

    assert(started_flag);
    assert(! thread_run_flag);

    thread_run_flag = true;
    NODE_LOG(info, "telling jackaudio thread to continue");
    thread_cond.notify_all();
}

// runs in a thread managed by jack audio
int_t jackaudio_node_t::handle_jack_process(const jackaudio_nframes_t nframes_in)
{
    NODE_LOG(info, "jackaudio thread gave us control");
    auto lock = get_object_lock();

    if (! started_flag) {
        thread_run_flag = false;
        thread_cond.notify_all();
        return false;
    }

    if (stopped_flag) {
        NODE_LOG(info, "jackaudio thread is returning because the node is stopped");
        thread_run_flag = false;
        thread_cond.notify_all();
        return true;
    }

    NODE_LOG(info, "jackaudio thread is running; nframes_in: ", nframes_in);

    auto const buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();

    if( buffer_size != nframes_in) {
        throw_runtime_error("jackaudio nframes read(", nframes_in, ") was not the same as the pcm buffer size: ", buffer_size);
    }

    for (auto i : sources) {
        auto source = dynamic_pointer_cast<audio_source_t>(i);
        auto portbuffer = get_port_buffer(source->name);
        auto buffer = jackalope::make_shared<audio_buffer_t>(buffer_size);

        pcm_copy(portbuffer, buffer->get_pointer(), buffer_size);
        source->notify_buffer(buffer);
    }

    NODE_LOG(info, "jackaudio thread is waiting for sinks to become ready");
    thread_cond.wait(lock, [&] { return stopped_flag || thread_run_flag; });
    NODE_LOG(info, "jackaudio thread woke up");

    thread_run_flag = false;
    thread_cond.notify_all();

    for(auto i : sinks) {
        auto sink = dynamic_pointer_cast<audio_sink_t>(i);
        auto portbuffer = get_port_buffer(sink->name);
        auto buffer = sink->get_buffer();

        sink->reset();
        pcm_copy(buffer->get_pointer(), portbuffer, buffer_size);
    }

    NODE_LOG(info, "jackaudio thread is done running");

    return false;
}

void jackaudio_node_t::stop()
{
    assert_lockable_owner();

    node_t::stop();

    log_info("waiting for jackaudio thread");
    thread_cond.notify_all();
    thread_cond.wait(object_mutex, [&] { return thread_run_flag == false; });
}

void jackaudio_node_t::open_client()
{
    assert_lockable_owner();

    auto client_name_property = get_property(JACKALOPE_AUDIO_JACKAUDIO_PROPERTY_CONFIG_CLIENT_NAME);
    auto set_client_name = client_name_property->get_string();
    auto sample_rate_property = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);

    assert(jack_client == nullptr);
    jack_client = jack_client_open(set_client_name.c_str(), jack_options, 0);

    if (jack_client == nullptr) {
        throw_runtime_error("could not open connection to jack server");
    }

    auto got_sample_rate = jack_get_sample_rate(jack_client);
    auto got_client_name = jack_get_client_name(jack_client);

    if (got_client_name != set_client_name) {
        client_name_property->set(got_client_name);
    }

    if (sample_rate_property->is_defined()) {
        if (sample_rate_property->get_size() != got_sample_rate) {
            throw_runtime_error("pcm:sample_rate did not match jackaudio server sample rate");
        }
    } else {
        sample_rate_property->set(got_sample_rate);
    }
}

jackaudio_port_t * jackaudio_node_t::add_port(const string_t& port_name_in, const char * port_type_in, const jackaudio_flags_t flags_in)
{
    assert_lockable_owner();

    if (jack_ports.count(port_name_in) != 0) {
        throw_runtime_error("duplicate port name: ", port_name_in);
    }

    auto new_port = jack_port_register(jack_client, port_name_in.c_str(), port_type_in, flags_in, 0);

    if (new_port == nullptr) {
        throw_runtime_error("could not create a jackaudio port named: ", port_name_in);
    }

    return jack_ports[port_name_in] = new_port;
}

real_t * jackaudio_node_t::get_port_buffer(const string_t& port_name_in)
{
    assert_lockable_owner();

    if (jack_ports.count(port_name_in) == 0) {
        throw_runtime_error("Unknown port name: ", port_name_in);
    }

    auto buffer = jack_port_get_buffer(jack_ports[port_name_in], get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size());

    assert(buffer != nullptr);
    return static_cast<real_t *>(buffer);
}

jackaudio_connection_daemon_t::jackaudio_connection_daemon_t(const string_t& type_in, const init_args_t& init_args_in)
: daemon_t(type_in, init_args_in)
{
    assert(type_in == JACKALOPE_AUDIO_JACKAUDIO_DAEMON_TYPE);
}

jackaudio_connection_daemon_t::~jackaudio_connection_daemon_t()
{
    if (jack_client != nullptr) {
        jack_client_close(jack_client);
        jack_client = nullptr;
    }
}

void jackaudio_connection_daemon_t::init()
{
    assert_lockable_owner();

    daemon_t::init();

    assert(jack_client == nullptr);
    jack_client = jack_client_open("Jackalope connection daemon", jackaudio::JackNoStartServer, 0);

    if (jack_client == nullptr) {
        throw_runtime_error("could not open connection to jack server");
    }
}

void jackaudio_connection_daemon_t::start()
{
    assert_lockable_owner();

    assert(jack_client != nullptr);

    daemon_t::start();

    if(jack_set_port_registration_callback(jack_client,[] (const uint32_t port_id_in, const int register_in, void * userdata_in) -> void {
        auto daemon = static_cast<jackaudio_connection_daemon_t *>(userdata_in);
        daemon->port_registration_callback(port_id_in, register_in);
    }, this)) {
        throw_runtime_error("could not set jack port registration callback");
    }

    if (jack_activate(jack_client)) {
        throw_runtime_error("could not activate jack client");
    }

    maintain_connections();
}

void jackaudio_connection_daemon_t::port_registration_callback(const uint32_t, const int register_in)
{
    // skip disconnect notifications
    if (! register_in) {
        return;
    }

    auto lock = get_object_lock();
    auto shared_this = shared_obj<jackaudio_connection_daemon_t>();

    if (update_pending) {
        return;
    }

    update_pending = true;

    // can't call jackaudio functions from inside the jackaudio
    // callback
    submit_job([shared_this] {
        auto lock = shared_this->get_object_lock();
        shared_this->maintain_connections();
    });
}

void jackaudio_connection_daemon_t::maintain_connections()
{
    assert_lockable_owner();

    update_pending = false;

    for(auto&& i : init_args) {
        auto& from = i.first;
        auto& to = i.second;

        log_info("connecting jackaudio ports: ", from, " -> ", to);
        jackaudio::jack_connect(jack_client, from.c_str(), to.c_str());
    }
}

void jackaudio_connection_daemon_t::stop()
{
    assert_lockable_owner();

    if (jack_client != nullptr) {
        jack_deactivate(jack_client);
    }

    daemon_t::stop();
}

} // namespace audio

} //namespace jackalope
