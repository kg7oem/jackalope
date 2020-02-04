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

#include <jackalope/jackalope.h>
#include <jackalope/pcm.h>
#include <jackalope/audio/rtaudio.h>

namespace jackalope {

namespace audio {

static shared_t<rtaudio_node_t> rtaudio_node_constructor(const string_t& type_in, const init_args_t init_args_in)
{
    assert(type_in == JACKALOPE_AUDIO_RTAUDIO_OBJECT_TYPE);

    return jackalope::make_shared<rtaudio_node_t>(init_args_in);
}

void rtaudio_init()
{
    add_object_constructor(JACKALOPE_AUDIO_RTAUDIO_OBJECT_TYPE, rtaudio_node_constructor);
}

int rtaudio_node_t::rtaudio_callback(void * output_buffer_in, void * input_buffer_in, unsigned int num_frames_in, double, RtAudioStreamStatus status_in, void * user_data_in)
{
    auto node = static_cast<rtaudio_node_t *>(user_data_in);
    return node->handle_rtaudio_process(output_buffer_in, input_buffer_in, num_frames_in, status_in);
}

rtaudio_node_t::rtaudio_node_t(const init_args_t init_args_in)
: threaded_driver_t(init_args_in)
{ }

rtaudio_node_t::~rtaudio_node_t()
{
    if (in_params != nullptr) {
        delete in_params;
        in_params = nullptr;
    }

    if (out_params != nullptr) {
        delete out_params;
        out_params = nullptr;
    }
}

void rtaudio_node_t::init() {
    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_AUDIO_RTAUDIO_PROPERTY_CONFIG_DEVICE_ID, property_t::type_t::size, init_args);

    adac.showWarnings(false);

    threaded_driver_t::init();
}

void rtaudio_node_t::activate() {
    assert_lockable_owner();

    for (auto i : { JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, JACKALOPE_PROPERTY_PCM_BUFFER_SIZE }) {
        set_undef_property(i);
    }

    auto device_id_property = get_property(JACKALOPE_AUDIO_RTAUDIO_PROPERTY_CONFIG_DEVICE_ID);
    auto sample_rate_property = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);
    auto buffer_size_property = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE);

    buffer_size = buffer_size_property->get_size();

    if (! device_id_property->is_defined()) {
        device_id_property->set(adac.getDefaultOutputDevice());
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

    auto num_sinks = get_num_sinks();
    auto num_sources = get_num_sources();

    if (num_sinks == 0 && num_sources == 0) {
        jackalope_panic("RtAudio node has no sources and no sinks");
    }

    if (num_sinks > 0) {
        out_params = new RtAudio::StreamParameters();
        out_params->deviceId = device_id_property->get_size();
        out_params->nChannels = num_sinks;
    }

    if (num_sources > 0) {
        in_params = new RtAudio::StreamParameters();
        in_params->deviceId = device_id_property->get_size();
        in_params->nChannels = num_sources;
    }

    try {
        adac.openStream(out_params, in_params, RTAUDIO_FLOAT32, sample_rate_property->get_size(), &buffer_size, &rtaudio_callback, (void *) this);
        adac.startStream();
    } catch (const RtAudioError& e) {
        jackalope_panic(e.getMessage());
    }

    threaded_driver_t::activate();
}

int rtaudio_node_t::handle_rtaudio_process(void * output_buffer_in, void * input_buffer_in, unsigned int num_frames_in, RtAudioStreamStatus)
{
    auto lock = get_object_lock();

    auto output_buffer = static_cast<real_t *>(output_buffer_in);
    auto input_buffer = static_cast<real_t *>(input_buffer_in);
    const auto num_sinks = get_num_sinks();
    const auto num_sources = get_num_sources();

    if (! started_flag) {
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();

        pcm_zero(output_buffer, num_frames_in);

        return 0;
    }

    if (stopped_flag) {
        driver_thread_run_flag = false;
        driver_thread_cond.notify_all();

        pcm_zero(output_buffer, num_frames_in);

        return 1;
    }

    driver_thread_cond.wait(lock, [&] { return stopped_flag || driver_thread_run_flag; });

    driver_thread_run_flag = false;
    driver_thread_cond.notify_all();

    if (stopped_flag) {
        return 1;
    }

    for (size_t i = 0; i < num_sources; i++) {
        auto source = get_source<audio_source_t>(i);
        auto buffer = jackalope::make_shared<audio_buffer_t>(buffer_size);

        pcm_extract_interleave(input_buffer, buffer->get_pointer(), i, num_sources, num_frames_in);
        source->notify_buffer(buffer);
    }

    for(size_t i = 0; i < num_sinks; i++) {
        auto sink = get_sink<audio_sink_t>(i);
        auto sink_buffer = sink->get_buffer();

        sink->reset();

        pcm_insert_interleave(sink_buffer->get_pointer(), output_buffer, i, num_sinks, num_frames_in);
    }

    return 0;
}

} // namespace audio

} //namespace jackalope
