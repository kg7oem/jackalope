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

static shared_t<portaudio_node_t> portaudio_driver_constructor(const init_args_t init_args_in)
{
    return jackalope::make_shared<portaudio_node_t>(init_args_in);
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

portaudio_node_t::portaudio_node_t(const init_args_t init_args_in)
: threaded_driver_plugin_t(init_args_in)
{ }

portaudio_node_t::~portaudio_node_t()
{
    auto lock = get_portaudio_lock();

    assert(stopped_flag);

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

    threaded_driver_plugin_t::init();
}

int process_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void *userdata_in)
{
    auto driver = static_cast<portaudio_node_t *>(userdata_in);
    return driver->process(input_buffer_in, output_buffer_in, frames_per_buffer_in, time_info_in, status_flags_in);
}

void portaudio_node_t::activate()
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

    auto sample_rate = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE)->get_size();
    auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();

    auto userdata = static_cast<void *>(this);

    threaded_driver_plugin_t::activate();

    auto lock = get_portaudio_lock();
    auto err = Pa_OpenDefaultStream(&stream, get_num_sources(), get_num_sinks(), paFloat32, sample_rate, buffer_size, process_cb, userdata);

    if (err != paNoError) {
        throw_runtime_error("Could not open portaudio default stream: ", Pa_GetErrorText(err));
    }

    assert(stream != nullptr);
    err = Pa_StartStream(stream);

    if (err != paNoError) {
        throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
    }
}

int portaudio_node_t::process(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in)
{
    NODE_LOG(info, "portaudio process() invoked");
    auto lock = get_object_lock();
    NODE_LOG(info, "portaudio process() got object lock");

    auto output_buffer = static_cast<real_t *>(output_buffer_in);
    auto input_buffer = static_cast<const real_t *>(input_buffer_in);
    auto num_sinks = get_num_sinks();
    auto num_sources = get_num_sources();

    pcm_zero(output_buffer, frames_per_buffer_in * num_sinks);

    if (! started_flag) {
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();

        return paContinue;
    }

    if (stopped_flag) {
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();

        return paAbort;
    }

    if (status_flags_in) {
        if (status_flags_in & paPrimingOutput) {
            // discard anything that is going to be used for priming
            driver_thread_run_flag = false;
            driver_thread_cond.notify_all();

            return paContinue;
        }

        if (status_flags_in & paInputUnderflow) {
            status_flags_in &= ~paInputUnderflow;
            log_info("portaudio input underflow for node ", name);
       }

        if (status_flags_in & paInputOverflow) {
            status_flags_in &= ~paInputOverflow;
            log_info("portaudio input overflow for node ", name);
        }

        if (status_flags_in & paOutputUnderflow) {
            status_flags_in &= ~paOutputUnderflow;
            log_info("portaudio output underflow for node ", name);
        }

        if (status_flags_in & paOutputOverflow) {
            status_flags_in &= ~paOutputOverflow;
            log_info("portaudio output overflow for node ", name);
        }

        if (status_flags_in) {
            throw_runtime_error("portaudio callback got unknown statusFlags: ", status_flags_in);
        }
    }

    for(size_t i = 0; i < num_sources; i++) {
        auto source = get_source(i)->shared_obj<audio_source_t>();
        auto buffer = jackalope::make_shared<audio_buffer_t>(frames_per_buffer_in);

        pcm_extract_interleave(input_buffer, buffer->get_pointer(), i, num_sources, frames_per_buffer_in);
        source->notify_buffer(buffer);
    }

    NODE_LOG(info, "PortAudio thread is waiting to run");
    driver_thread_cond.wait(lock, [this] { return stopped_flag || driver_thread_run_flag; });
    NODE_LOG(info, "PortAudio is done waiting to run");

    driver_thread_run_flag = false;
    driver_thread_cond.notify_all();

    if (stopped_flag) {
        NODE_LOG(info, "portaudio thread is returning because the node is stopped");
        return paAbort;
    }

    for(size_t i = 0; i < num_sinks; i++) {
        auto sink = get_sink(i)->shared_obj<audio_sink_t>();

        auto buffer = sink->get_buffer();
        sink->reset();

        pcm_insert_interleave(buffer->get_pointer(), output_buffer, i, num_sinks, frames_per_buffer_in);
    }

    NODE_LOG(info, "portaudio thread is done");

    return paContinue;
}

} // namespace pcm

} // namespace jackalope
