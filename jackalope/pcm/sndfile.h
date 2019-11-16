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

#include <jackalope/pcm.h>

#define JACKALOPE_PCM_SNDFILE_CLASS "pcm::sndfile"
#define JACKALOPE_PCM_SNDFILE_CONFIG_PATH "config:path"

namespace jackalope {

namespace pcm {

namespace sndfile {

extern "C" {
#include <sndfile.h>
}

} // namespace sndfile

using sndfile_handle_t = sndfile::SNDFILE;
using sndfile_info_t = sndfile::SF_INFO;

void sndfile_init();

struct sndfile_node_t : public pcm_node_t {
    const string_t class_name = JACKALOPE_PCM_SNDFILE_CLASS;

    sndfile_handle_t *source_file = nullptr;
    sndfile_info_t source_info;
    real_t * source_buffer;

    sndfile_node_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~sndfile_node_t();
    void activate() override;
    void pcm_ready() override;
    void close_file(sndfile_handle_t * file_in);
};

} // namespace pcm

} // namespace jackalope
