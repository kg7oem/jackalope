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

#include <jackalope/channel.h>
#include <jackalope/driver.h>
#include <jackalope/pcm.h>

#define JACKALOPE_PORTAUDIO_DRIVER_CLASS "pcm::portaudio"

namespace jackalope {

namespace pcm {

using portaudio_stream_t = PaStream;
using portaudio_stream_cb_time_info_t = PaStreamCallbackTimeInfo;
using portaudio_stream_cb_flags = PaStreamCallbackFlags;

void portaudio_init();

class portaudio_driver_t : public driver_t {

protected:
    const string_t class_name = JACKALOPE_PORTAUDIO_DRIVER_CLASS;
    portaudio_stream_t * stream = nullptr;
    pool_list_t<weak_t<sink_t>> sinks;
    pool_list_t<weak_t<source_t>> sources;

    virtual void init__e() override;
    virtual void activate__e() override;
    virtual void start__e() override;

public:
    portaudio_driver_t(const init_list_t& init_list_in);
    virtual ~portaudio_driver_t();
    virtual int process(const void *input_buffer_in, void *output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *time_info_in, portaudio_stream_cb_flags status_flags_in);
};

} // namespace pcm

} // namespace jackalope
