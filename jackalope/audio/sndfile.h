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

#include <jackalope/audio/pcm.h>
#include <jackalope/plugin.h>

#define JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_AHEAD          262144 // bytes
#define JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_SIZE           16384 // bytes
#define JACKALOPE_AUDIO_SNDFILE_OBJECT_TYPE                 "audio::sndfile"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH        "config.path"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD         "config.read_ahead"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE          "config.read_size"

namespace jackalope {

namespace audio {

namespace sndfile {

extern "C" {
#include <sndfile.h>
}

} // namespace sndfile

using sndfile_handle_t = sndfile::SNDFILE;
using sndfile_info_t = sndfile::SF_INFO;

class sndfile_plugin_t : public driver_plugin_t {

protected:
    sndfile_handle_t * source_file = nullptr;
    sndfile_info_t source_info;

    virtual void execute() override;
    virtual void open_file();
    virtual void close_file();

public:
    static inline const string_t type = "audio::sndfile";

    static shared_t<plugin_t> make(shared_t<project_t> project_in, const init_args_t& init_args_in);
    sndfile_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual ~sndfile_plugin_t();
    virtual const string_t& get_type() override;
    virtual void will_init() override;
    virtual void did_init() override;
    virtual void will_activate() override;
    virtual void will_stop() override;
};

} // namespace audio

} //namespace jackalope
