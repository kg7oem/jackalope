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

static shared_t<rtaudio_node_t> rtaudio_node_constructor(const init_args_t init_args_in)
{
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
: plugin_t(init_args_in)
{ }

void rtaudio_node_t::init() {
    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_AUDIO_RTAUDIO_PROPERTY_CONFIG_DEVICE_ID, property_t::type_t::size, init_args);

    plugin_t::init();
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

    for (auto& i : init_args_find("sink", init_args)) {
        auto sink_name = split_string(i.first, '.').at(1);
        auto sink_type = i.second;
        add_sink(sink_name, sink_type);
    }

    out_params.deviceId = device_id_property->get_size();
    out_params.nChannels = get_num_sinks();

    try {
        adac.openStream(&out_params, nullptr, RTAUDIO_FLOAT32, sample_rate_property->get_size(), &buffer_size, &rtaudio_callback, (void *) this);
        adac.startStream();
    } catch (const RtAudioError& e) {
        jackalope_panic(e.getMessage());
    }

    plugin_t::activate();
}

void rtaudio_node_t::start() {
    assert_lockable_owner();

    plugin_t::start();
}

bool rtaudio_node_t::should_execute() {
    assert_lockable_owner();

    if (rtaudio_run_flag) {
        return false;
    }

    for (auto i : sinks) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

void rtaudio_node_t::execute() {
    assert_lockable_owner();

    assert(started_flag);
    assert(! rtaudio_run_flag);

    rtaudio_run_flag = true;
    rtaudio_cond.notify_all();
}

int rtaudio_node_t::handle_rtaudio_process(void * output_buffer_in, void *, unsigned int num_frames_in, RtAudioStreamStatus)
{
    auto lock = get_object_lock();

    auto output_buffer = static_cast<real_t *>(output_buffer_in);
    const auto num_sinks = get_num_sinks();

    if (! started_flag) {
        rtaudio_run_flag = false;
        rtaudio_cond.notify_all();

        pcm_zero(output_buffer, num_frames_in);

        return 0;
    }

    if (stopped_flag) {
        rtaudio_run_flag = false;
        rtaudio_cond.notify_all();

        pcm_zero(output_buffer, num_frames_in);

        return 1;
    }

    rtaudio_cond.wait(lock, [&] { return stopped_flag || rtaudio_run_flag; });

    rtaudio_run_flag = false;
    rtaudio_cond.notify_all();

    if (stopped_flag) {
        return 1;
    }

    for(size_t i = 0; i < num_sinks; i++) {
        auto sink = dynamic_pointer_cast<audio_sink_t>(get_sink(i));
        auto sink_buffer = sink->get_buffer();

        sink->reset();

        pcm_insert_interleave(sink_buffer->get_pointer(), output_buffer, i, num_sinks, num_frames_in);
    }

    return 0;
}

void rtaudio_node_t::stop()
{
    assert_lockable_owner();

    plugin_t::stop();

    rtaudio_cond.notify_all();
    rtaudio_cond.wait(object_mutex, [&] { return rtaudio_run_flag == false; });
}

} // namespace audio

} //namespace jackalope
