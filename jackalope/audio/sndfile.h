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
#include <jackalope/pcm.h>

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

void sndfile_init();

class sndfile_node_t : public plugin_t {

protected:
    sndfile_handle_t * source_file = nullptr;
    sndfile_info_t source_info;
    thread_t * io_thread = nullptr;
    pool_list_t<shared_t<audio_buffer_t>> thread_work;
    condition_t thread_work_cond;
    bool had_short_copy_flag = false;

    virtual void be_io_thread();
    virtual void add_work(shared_t<audio_buffer_t>);
    virtual void wait_work_available();
    virtual bool should_execute() override;
    virtual void execute() override;
    virtual void close_file();

public:
    sndfile_node_t(const init_args_t init_args_in);
    virtual ~sndfile_node_t();
    virtual void init() override;
    virtual void activate() override;
    virtual void start() override;
    virtual void stop() override;
};

} // namespace pcm

} // namespace jackalope
