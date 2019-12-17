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

#include <jackalope/pcm.h>
#include <jackalope/types.h>

namespace jackalope {

static shared_t<source_t> pcm_source_real_constructor(const string_t& name_in, const string_t& type_in)
{
    return jackalope::make_shared<pcm_source_t<real_t>>(name_in, type_in);
}

static shared_t<sink_t> pcm_sink_real_constructor(const string_t& name_in, const string_t& type_in)
{
    return jackalope::make_shared<pcm_sink_t<real_t>>(name_in, type_in);
}

void pcm_init()
{
    add_source_constructor(JACKALOPE_PCM_CHANNEL_TYPE_REAL, pcm_source_real_constructor);
    add_sink_constructor(JACKALOPE_PCM_CHANNEL_TYPE_REAL, pcm_sink_real_constructor);
}

} // namespace jackalope
