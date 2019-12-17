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

#include <jackalope/channel.h>
#include <jackalope/pcm.tools.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_CHANNEL_TYPE_REAL    "pcm[real]"
#define JACKALOPE_PCM_CHANNEL_TYPE_QUAD    "pcm[quad]"
#define JACKALOPE_PCM_PROPERTY_BUFFER_SIZE  "pcm:buffer_size"
#define JACKALOPE_PCM_PROPERTY_SAMPLE_RATE  "pcm:sample_rate"

namespace jackalope {

void pcm_init();

template <typename T>
class pcm_source_t : public source_t {

public:
    pcm_source_t(const string_t& name_in, const string_t& type_in)
    : source_t(name_in, type_in)
    { }
};

template <typename T>
class pcm_sink_t : public sink_t {

public:
    pcm_sink_t(const string_t& name_in, const string_t& type_in)
    : sink_t(name_in, type_in)
    { }
};

} // namespace jackalope
