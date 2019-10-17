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

namespace jackalope {

namespace audio {

static mutex_t portaudio_mutex;

static lock_t get_portaudio_lock()
{
    return lock_t(portaudio_mutex);
}

void portaudio_init()
{
    auto lock = get_portaudio_lock();
    auto err = Pa_Initialize();

    if (err != paNoError) {
        throw_runtime_error("Could not initialize portaudio: ",  Pa_GetErrorText(err));
    }
}

portaudio_driver_t::portaudio_driver_t(const string_t& name_in, audio_domain_t * domain_in)
: audio_driver_t(JACKALOPE_AUDIO_PORTAUDIO_CLASS, name_in, domain_in)
{
    add_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
    add_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
}

portaudio_driver_t::~portaudio_driver_t()
{
    if (stream != nullptr) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
}

void portaudio_driver_t::init()
{
    get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(domain->get_buffer_size());
    get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(domain->get_sample_rate());

    add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "left input");
    add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "right input");

    audio_driver_t::init();
}

static int process_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void *userdata_in)
{
    auto driver = static_cast<portaudio_driver_t *>(userdata_in);
    return driver->process(input_buffer_in, output_buffer_in, frames_per_buffer_in, time_info_in, status_flags_in);
}

void portaudio_driver_t::activate()
{
    auto lock = get_portaudio_lock();
    auto userdata = static_cast<void *>(this);
    auto num_samples = get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).get_size();
    auto sample_rate = get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).get_size();
    auto err = Pa_OpenDefaultStream(&stream, outputs.size(), inputs.size(), paFloat32, sample_rate, num_samples, process_cb, userdata);

    if (err != paNoError) {
        throw_runtime_error("Could not open portaudio default stream: ", Pa_GetErrorText(err));
    }

    audio_driver_t::activate();
}

void portaudio_driver_t::start()
{
    auto lock = get_portaudio_lock();

    assert(stream != nullptr);

    auto err = Pa_StartStream(stream);

    if (err != paNoError) {
        throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
    }
}

int portaudio_driver_t::process(const void * source_buffer_in, void * sink_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in)
{
    auto lock = get_portaudio_lock();
    auto source_buffer = static_cast<const real_t *>(source_buffer_in);
    auto sink_buffer = static_cast<real_t *>(sink_buffer_in);

    if (frames_per_buffer_in != get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).get_size()) {
        throw_runtime_error("portaudio frames per buffer did not match buffer size: ", frames_per_buffer_in);
    }

    assert(source_buffer == nullptr);
    assert(sink_buffer != nullptr);

    if (status_flags_in) {
        if (status_flags_in & paPrimingOutput) {
            // discard anything that is going to be used for priming
            return 0;
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

    throw_runtime_error("portaudio needs to deinterlace");

    return 0;
}

void portaudio_driver_t::input_ready(input_t&)
{
    throw_runtime_error("portaudio driver can't handle an input being ready");
}

void portaudio_driver_t::notify()
{
    throw_runtime_error("portaudio driver can not handle notifying");
}

} // namespace audio

} // namespace jackalope
