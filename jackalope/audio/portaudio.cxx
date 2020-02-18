// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/audio/channel.h>
#include <jackalope/audio/pcm.h>
#include <jackalope/audio/portaudio.h>
#include <jackalope/logging.h>

#define JACKALOPE_AUDIO_PORTAUDIO_OBJECT_TYPE "audio::portaudio"

namespace jackalope {

namespace audio {

static bool portaudio_init_flag = false;

static lock_t get_portaudio_lock()
{
    static mutex_t portaudio_mutex;
    return lock_t(portaudio_mutex);
}

static void init_portaudio()
{
    auto lock = get_portaudio_lock();

    if (! portaudio_init_flag) {
        auto err = Pa_Initialize();

        if (err != paNoError) {
            throw_runtime_error("Could not initialize portaudio: ",  Pa_GetErrorText(err));
        }

        portaudio_init_flag = true;
    }
}

shared_t<portaudio_plugin_t> portaudio_plugin_t::make(shared_t<project_t> project_in, const init_args_t& init_args_in)
{
    init_portaudio();

    return object_t::make<portaudio_plugin_t>(project_in, init_args_in);
}

portaudio_plugin_t::portaudio_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: driver_plugin_t(project_in, init_args_in)
{ }

const string_t& portaudio_plugin_t::get_type()
{
    return type;
}

void portaudio_plugin_t::will_init()
{
    assert_lockable_owner();

    add_channel_type(audio_channel_info_t::type);
    add_init_args_channels();

    driver_plugin_t::will_init();
}

void portaudio_plugin_t::will_activate()
{
    assert_lockable_owner();

    auto num_sources = sources.size();
    auto num_sinks = sinks.size();
    auto sample_rate = get_property(JACKALOPE_PROPERTY_AUDIO_SAMPLE_RATE)->get_size();
    auto buffer_size = get_property(JACKALOPE_PROPERTY_AUDIO_BUFFER_SIZE)->get_size();

    {
        auto lock = get_portaudio_lock();
        auto err = Pa_OpenDefaultStream(&stream, num_sources, num_sinks, paFloat32, sample_rate, buffer_size, portaudio_cb, this);

        if (err != paNoError) {
            throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
        }

        err = Pa_StartStream(stream);

        if (err != paNoError) {
            throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
        }
    }

    driver_plugin_t::will_activate();
}

int portaudio_plugin_t::portaudio_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void * userdata_in)
{
    auto plugin = (portaudio_plugin_t *)userdata_in;
    return plugin->process(input_buffer_in, output_buffer_in, frames_per_buffer_in, time_info_in, status_flags_in);
}

void portaudio_plugin_t::execute()
{
    assert_lockable_owner();

    assert(wait_cb_flag == false);

    portaudio_condition.notify_all();

    wait_cb_flag = true;
    object_log_trace("Portaudio plugin is waiting for callback");
    portaudio_condition.wait(object_mutex, [&] { return stopped_flag || ! wait_cb_flag; });
    object_log_trace("Portaudio plugin is done waiting for callback");
}

int portaudio_plugin_t::process(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in)
{
    auto lock = get_object_lock();

    auto output_buffer = static_cast<real_t *>(output_buffer_in);
    auto input_buffer = static_cast<const real_t *>(input_buffer_in);
    auto num_sources = sources.size();
    auto num_sinks = sinks.size();
    auto buffer_size = get_property(JACKALOPE_PROPERTY_AUDIO_BUFFER_SIZE)->get_size();
    int retval;

    if (buffer_size != frames_per_buffer_in) {
        throw_runtime_error("portaudio frames_per_buffer != buffer_size");
    }

    pcm_zero(output_buffer, frames_per_buffer_in * num_sinks);

    // check_status_flags(status_flags_in);

    if (status_flags_in & paPrimingOutput || ! running_flag) {
        // discard anything that is going to be used for priming
        retval = paContinue;
        goto cleanup;
    } else if (stopped_flag) {
        retval = paAbort;
        goto cleanup;
    }

    object_log_trace("Portaudio callback is waiting for sources to become available");
    portaudio_condition.wait(lock, [&] { return stopped_flag || sources_are_available(); });
    object_log_trace("Portaudio callback is done waiting for sources to become available");

    if (stopped_flag) {
        retval = paAbort;
        goto cleanup;
    }

    for(size_t i = 0; i < num_sources; i++) {
        auto source = get_source<audio_source_t>(i);
        auto buffer = audio_buffer_t::make(buffer_size);

        pcm_extract_interleave(input_buffer, buffer->get_pointer(), i, num_sources, frames_per_buffer_in);
        source->notify_buffer(buffer);
    }

    object_log_trace("Portaudio callback is waiting for sinks to become ready");
    portaudio_condition.wait(lock, [&] { return stopped_flag || sinks_are_ready(); });
    object_log_trace("Portaudio callback is done waiting for sinks to become ready");

    if (stopped_flag) {
        retval = paAbort;
        goto cleanup;
    }

    for(size_t i = 0; i < num_sinks; i++) {
        auto sink = get_sink<audio_sink_t>(i);
        auto buffer = sink->get_buffer();

        pcm_insert_interleave(buffer->get_pointer(), output_buffer, i, num_sources, frames_per_buffer_in);
        sink->reset();
    }

    retval = paContinue;

cleanup:
    wait_cb_flag = false;
    portaudio_condition.notify_all();
    return retval;
}

} // namespace audio

} //namespace jackalope
