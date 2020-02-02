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

#include <jackalope/audio.h>
#include <jackalope/plugin.h>
#include <jackalope/types.h>

#define JACKALOPE_AUDIO_JACKAUDIO_OBJECT_TYPE "audio::jackaudio"
#define JACKALOPE_AUDIO_JACKAUDIO_PROPERTY_CONFIG_CLIENT_NAME "config.client_name"

namespace jackalope {

namespace audio {

namespace jackaudio {

extern "C" {
#include <jack/jack.h>
}

} // namespace jackaudio

using jackaudio_client_t = jackaudio::jack_client_t;
using jackaudio_flags_t = unsigned long;
using jackaudio_nframes_t = jackaudio::jack_nframes_t;
using jackaudio_options_t = jackaudio::jack_options_t;
using jackaudio_port_t = jackaudio::jack_port_t;

void jackaudio_init();

class jackaudio_node_t : public plugin_t {

protected:
    jackaudio_client_t * jack_client = nullptr;
    const jackaudio_options_t jack_options = jackaudio::JackNoStartServer;
    pool_map_t<string_t, jackaudio_port_t *> jack_ports;
    condition_t thread_cond;
    bool thread_run_flag = false;

    virtual void open_client();
    virtual int_t handle_jack_process(const jackaudio_nframes_t num_frames_in);

public:
    jackaudio_node_t(const init_args_t init_args_in);
    virtual ~jackaudio_node_t();
    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in) override;
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in) override;
    virtual jackaudio_port_t * add_port(const string_t& port_name_in, const char * port_type_in, const jackaudio_flags_t flags_in);
    virtual real_t * get_port_buffer(const string_t& port_name_in);
    virtual void init() override;
    virtual void activate() override;
    virtual void start() override;
    virtual bool should_execute() override;
    virtual void execute() override;
    virtual void stop() override;
};

} // namespace audio

} //namespace jackalope
