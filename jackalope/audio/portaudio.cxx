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

#include <jackalope/audio/portaudio.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/pcm.h>
#include <jackalope/string.h>

namespace jackalope {

namespace audio {

static mutex_t portaudio_mutex;

static lock_t get_portaudio_lock()
{
    return lock_t(portaudio_mutex);
}

static shared_t<portaudio_node_t> portaudio_driver_constructor(const init_list_t& init_list_in)
{
    return jackalope::make_shared<portaudio_node_t>(init_list_in);
}

void portaudio_init()
{
    auto lock = get_portaudio_lock();
    auto err = Pa_Initialize();

    if (err != paNoError) {
        throw_runtime_error("Could not initialize portaudio: ",  Pa_GetErrorText(err));
    }

    add_object_constructor(JACKALOPE_AUDIO_PORTAUDIO_OBJECT_TYPE, portaudio_driver_constructor);
}

portaudio_node_t::portaudio_node_t(const init_list_t& init_list_in)
: node_t(init_list_in)
{ }

portaudio_node_t::~portaudio_node_t()
{
    if (stream != nullptr) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
}

void portaudio_node_t::init()
{
    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);

    node_t::init();
}

int process_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void *userdata_in)
{
    auto driver = static_cast<portaudio_node_t *>(userdata_in);
    return driver->process(input_buffer_in, output_buffer_in, frames_per_buffer_in, time_info_in, status_flags_in);
}

void portaudio_node_t::activate()
{
    assert_lockable_owner();

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

    auto sample_rate = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE)->get_size();
    auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();

    auto lock = get_portaudio_lock();
    auto userdata = static_cast<void *>(this);

    auto err = Pa_OpenDefaultStream(&stream, sources.size(), sinks.size(), paFloat32, sample_rate, buffer_size, process_cb, userdata);

    if (err != paNoError) {
        throw_runtime_error("Could not open portaudio default stream: ", Pa_GetErrorText(err));
    }

    node_t::activate();
}

void portaudio_node_t::start()
{
    auto lock = get_portaudio_lock();

    assert(stream != nullptr);

    node_t::start();

    // auto err = Pa_StartStream(stream);

    // if (err != paNoError) {
    //     throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
    // }
}

bool portaudio_node_t::should_run()
{
    assert_lockable_owner();

    for(auto i : sinks) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

void portaudio_node_t::run()
{
    NODE_LOG(info, "portaudio node running");

    for(auto i : sinks) {
        auto audio_sink = i->shared_obj<audio_sink_t>();
        i->reset();
    }

    check_sinks_ready();
}

int portaudio_node_t::process(const void *, void * sink_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in)
{
    auto obj_lock = get_object_lock();
    auto pa_lock = get_portaudio_lock();
    // auto source_buffer = static_cast<const real_t *>(source_buffer_in);
    auto sink_buffer = static_cast<real_t *>(sink_buffer_in);

    if (frames_per_buffer_in != get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size()) {
        throw_runtime_error("portaudio frames per buffer did not match buffer size: ", frames_per_buffer_in);
    }

    if (status_flags_in) {
        if (status_flags_in & paPrimingOutput) {
            // discard anything that is going to be used for priming
            return 0;
        }

        if (status_flags_in & paInputUnderflow) {
            status_flags_in &= ~paInputUnderflow;
            // log_info("portaudio input underflow for node ", name);
       }

        if (status_flags_in & paInputOverflow) {
            status_flags_in &= ~paInputOverflow;
            // log_info("portaudio input overflow for node ", name);
        }

        if (status_flags_in & paOutputUnderflow) {
            status_flags_in &= ~paOutputUnderflow;
            // log_info("portaudio output underflow for node ", name);
        }

        if (status_flags_in & paOutputOverflow) {
            status_flags_in &= ~paOutputOverflow;
            // log_info("portaudio output overflow for node ", name);
        }

        if (status_flags_in) {
            throw_runtime_error("portaudio callback got unknown statusFlags: ", status_flags_in);
        }
    }

    // auto sources_count = sources.size();
    auto sinks_count = sinks.size();
    size_t channel;

    // channel = 0;
    // for(auto& i : sources) {
    //     auto source = dynamic_pointer_cast<pcm_source_t<real_t>>(i.lock());
    //     auto& buffer = source->get_buffer();

    //     pcm_extract_interleave(source_buffer, buffer.get_pointer(), channel, sources_count, frames_per_buffer_in);

    //     channel++;
    // }

    channel = 0;
    for(auto& i : sinks) {
        auto sink = i->shared_obj<audio_sink_t>();
        auto buffer = sink->get_buffer();

        pcm_insert_interleave(buffer->get_pointer(), sink_buffer, channel, sinks_count, frames_per_buffer_in);

        channel++;
    }

    return 0;
}

} // namespace pcm

} // namespace jackalope
