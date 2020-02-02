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

#pragma once

#include <rtaudio/RtAudio.h>

#include <jackalope/audio.h>
#include <jackalope/plugin.h>
#include <jackalope/types.h>

#define JACKALOPE_AUDIO_RTAUDIO_OBJECT_TYPE "audio::rtaudio"
#define JACKALOPE_AUDIO_RTAUDIO_PROPERTY_CONFIG_DEVICE_ID "config.device_id"

namespace jackalope {

namespace audio {

void rtaudio_init();

class rtaudio_node_t : public threaded_driver_plugin_t {

protected:
    RtAudio adac;
    RtAudio::StreamParameters * out_params = nullptr;
    RtAudio::StreamParameters * in_params = nullptr;
    unsigned int buffer_size = 0;

    static int rtaudio_callback(void * output_buffer_in, void * input_buffer_in, unsigned int num_frames_in, double stream_time_in, RtAudioStreamStatus status_in, void * user_data_in);
    virtual int handle_rtaudio_process(void * output_buffer_in, void * input_buffer_in, unsigned int num_frames_in, RtAudioStreamStatus status_in);

public:
    rtaudio_node_t(const init_args_t init_args_in);
    ~rtaudio_node_t();

    virtual void init() override;
    virtual void activate() override;
};

} // namespace audio

} //namespace jackalope
