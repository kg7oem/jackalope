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

#include <jackalope/domain.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/pcm/portaudio.h>

namespace jackalope {

namespace pcm {

static mutex_t portaudio_mutex;

static lock_t get_portaudio_lock()
{
    return lock_t(portaudio_mutex);
}

static shared_t<portaudio_driver_t> portaudio_driver_constructor(const init_list_t init_list_in)
{
    return jackalope::make_shared<portaudio_driver_t>(init_list_in);
}

void portaudio_init()
{
    auto lock = get_portaudio_lock();
    auto err = Pa_Initialize();

    if (err != paNoError) {
        throw_runtime_error("Could not initialize portaudio: ",  Pa_GetErrorText(err));
    }

    add_driver_constructor(JACKALOPE_PORTAUDIO_DRIVER_CLASS, portaudio_driver_constructor);
}

portaudio_driver_t::portaudio_driver_t(const init_list_t& init_list_in)
: driver_t(init_list_in)
{ }

portaudio_driver_t::~portaudio_driver_t()
{
    if (stream != nullptr) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
}

void portaudio_driver_t::init__e()
{
    assert_lockable_owner();

    add_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
    add_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, property_t::type_t::size);

    driver_t::init__e();
}

int process_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void *userdata_in)
{
    auto driver = static_cast<portaudio_driver_t *>(userdata_in);
    return driver->process(input_buffer_in, output_buffer_in, frames_per_buffer_in, time_info_in, status_flags_in);
}

void portaudio_driver_t::activate__e()
{
    assert_lockable_owner();

    auto domain = get_domain__e();
    auto& sample_rate_prop = domain->get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE);
    auto& buffer_size_prop = domain->get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

    if (! sample_rate_prop.is_defined()) {
        throw_runtime_error("domain sample rate was not defined");
    }

    if (! buffer_size_prop.is_defined()) {
        throw_runtime_error("domain buffer size was not defined");
    }

    auto buffer_size = buffer_size_prop.get_size();
    auto sample_rate = sample_rate_prop.get_size();

    get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE).set(sample_rate);
    get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).set(buffer_size);

    for (auto& i : init_list_find("source", init_args)) {
        auto source_name = split_string(i.first, ':').at(1);
        auto source_type = i.second;
        auto source = get_domain__e()->add_source(source_name, source_type);
        sources.push_back(source);
    }

    for (auto& i : init_list_find("sink", init_args)) {
        auto sink_name = split_string(i.first, ':').at(1);
        auto sink_type = i.second;
        auto sink = get_domain__e()->add_sink(sink_name, sink_type);
        sinks.push_back(sink);
    }

    auto lock = get_portaudio_lock();
    auto userdata = static_cast<void *>(this);

    auto err = Pa_OpenDefaultStream(&stream, sources.size(), sinks.size(), paFloat32, sample_rate, buffer_size, process_cb, userdata);

    if (err != paNoError) {
        throw_runtime_error("Could not open portaudio default stream: ", Pa_GetErrorText(err));
    }

    driver_t::activate__e();
}

void portaudio_driver_t::start__e()
{
    auto lock = get_portaudio_lock();

    assert(stream != nullptr);

    auto err = Pa_StartStream(stream);

    if (err != paNoError) {
        throw_runtime_error("Could not start portaudio stream: ", Pa_GetErrorText(err));
    }

    driver_t::start__e();
}

int portaudio_driver_t::process(const void * source_buffer_in, void * sink_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in)
{
    auto obj_lock = get_object_lock();
    auto pa_lock = get_portaudio_lock();
    auto source_buffer = static_cast<const real_t *>(source_buffer_in);
    auto sink_buffer = static_cast<real_t *>(sink_buffer_in);

    if (frames_per_buffer_in != get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).get_size()) {
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

    auto sources_count = sources.size();
    auto sinks_count = sinks.size();
    size_t channel;

    channel = 0;
    for(auto& i : sources) {
        auto source = dynamic_pointer_cast<pcm_source_t<real_t>>(i.lock());
        auto& buffer = source->get_buffer();

        pcm_extract_interleave(source_buffer, buffer.get_pointer(), channel, sources_count, frames_per_buffer_in);

        channel++;
    }

    // FIXME the nodes need to run here

    channel = 0;
    for(auto& i : sinks) {
        auto sink = dynamic_pointer_cast<pcm_sink_t<real_t>>(i.lock());
        auto& buffer = sink->get_buffer();

        pcm_insert_interleave(buffer.get_pointer(), sink_buffer, channel, sinks_count, frames_per_buffer_in);

        channel++;
    }

    return 0;
}

} // namespace pcm

} // namespace jackalope
