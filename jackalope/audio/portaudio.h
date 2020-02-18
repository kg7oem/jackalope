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

#pragma once

#include <jackalope/plugin.h>
#include <jackalope/types.h>

extern "C" {
#include <portaudio.h>
}

namespace jackalope {

namespace audio {

using portaudio_stream_t = PaStream;
using portaudio_stream_cb_time_info_t = PaStreamCallbackTimeInfo;
using portaudio_stream_cb_flags = PaStreamCallbackFlags;

class portaudio_plugin_t : public driver_plugin_t {

protected:
    portaudio_stream_t * stream = nullptr;
    condition_t portaudio_condition;
    bool wait_cb_flag = false;

    static int portaudio_cb(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t * time_info_in, portaudio_stream_cb_flags status_flags_in, void * userdata_in);
    virtual void will_init() override;
    virtual void will_activate() override;
    virtual int process(const void * input_buffer_in, void * output_buffer_in, size_t frames_per_buffer_in, const portaudio_stream_cb_time_info_t *, portaudio_stream_cb_flags status_flags_in);
    virtual void execute() override;

public:
    static inline const string_t& type = "audio::portaudio";

    static shared_t<portaudio_plugin_t> make(shared_t<project_t> project_in, const init_args_t& init_args_in);
    portaudio_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual ~portaudio_plugin_t() = default;
    virtual const string_t& get_type() override;
};

} // namespace audio

} //namespace jackalope
