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

extern "C" {
#include <portaudio.h>
}

#include <jackalope/audio.h>
#include <jackalope/node.h>

#define JACKALOPE_AUDIO_PORTAUDIO_OBJECT_TYPE "audio::portaudio"

namespace jackalope {

namespace audio {

using portaudio_stream_t = PaStream;
using portaudio_stream_cb_time_info_t = PaStreamCallbackTimeInfo;
using portaudio_stream_cb_flags = PaStreamCallbackFlags;

void portaudio_init();

class portaudio_node_t : public node_t {

protected:
    portaudio_stream_t * stream = nullptr;
    bool thread_run = false;
    condition_t thread_run_cond;

    virtual void init() override;
    virtual void activate() override;
    virtual void start() override;
    virtual bool should_run() override;
    virtual void run() override;
    virtual void stop() override;

public:
    portaudio_node_t(const init_list_t& init_list_in);
    virtual ~portaudio_node_t();
    virtual int process(const void * source_buffer_in, void * sink_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *time_info_in, portaudio_stream_cb_flags status_flags_in);
};

} // namespace pcm

} // namespace jackalope
