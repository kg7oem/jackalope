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

static shared_t<jackaudio_node_t> jackaudio_node_constructor(NDEBUG_UNUSED const string_t& type_in, const init_args_t init_args_in)
{
    assert(type_in == JACKALOPE_AUDIO_JACKAUDIO_OBJECT_TYPE);

    return jackalope::make_shared<jackaudio_node_t>(init_args_in);
}

void jackaudio_init()
{
    add_object_constructor(JACKALOPE_AUDIO_JACKAUDIO_OBJECT_TYPE, jackaudio_node_constructor);
}

jackaudio_node_t::jackaudio_node_t(const init_args_t init_args_in)
: threaded_driver_t(init_args_in)
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

    return threaded_driver_t::add_source(source_name_in, type_in);
}

shared_t<sink_t> jackaudio_node_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sources_by_name.count(sink_name_in) != 0) {
        throw_runtime_error("jackaudio sinks can not have same name as sources: ", sink_name_in);
    }

    return threaded_driver_t::add_sink(sink_name_in, type_in);
}

void jackaudio_node_t::init()
{
    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_AUDIO_JACKAUDIO_PROPERTY_CONFIG_CLIENT_NAME, property_t::type_t::string, init_args);

    return threaded_driver_t::init();
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

    threaded_driver_t::activate();

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

// runs in a thread managed by jack audio
int_t jackaudio_node_t::handle_jack_process(const jackaudio_nframes_t nframes_in)
{
    object_log_info("jackaudio thread gave us control");
    auto lock = get_object_lock();

    if (! started_flag) {
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();
        return false;
    }

    if (stopped_flag) {
        object_log_info("jackaudio thread is returning because the node is stopped");
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();
        return true;
    }

    object_log_info("jackaudio thread is running; nframes_in: ", nframes_in);

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

    object_log_info("jackaudio thread is waiting for sinks to become ready");
    driver_thread_cond.wait(lock, [&] { return stopped_flag || driver_thread_run_flag; });
    object_log_info("jackaudio thread woke up");

    driver_thread_run_flag = false;
    driver_thread_cond.notify_all();

    for(auto i : sinks) {
        auto sink = dynamic_pointer_cast<audio_sink_t>(i);
        auto portbuffer = get_port_buffer(sink->name);
        auto buffer = sink->get_buffer();

        sink->reset();
        pcm_copy(buffer->get_pointer(), portbuffer, buffer_size);
    }

    object_log_info("jackaudio thread is done running");

    return false;
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

} // namespace audio

} //namespace jackalope
