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

#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.h>

namespace jackalope {

static input_t * pcm_real_input_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_real_input_t(name_in, parent_in);
}

static output_t * pcm_real_output_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_real_output_t(name_in, parent_in);
}

static input_t * pcm_quad_input_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_quad_input_t(name_in, parent_in);
}

static output_t * pcm_quad_output_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_quad_output_t(name_in, parent_in);
}

void pcm_init()
{
    add_input_constructor(JACKALOPE_PCM_CHANNEL_CLASS_REAL, pcm_real_input_constructor);
    add_output_constructor(JACKALOPE_PCM_CHANNEL_CLASS_REAL, pcm_real_output_constructor);

    add_input_constructor(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, pcm_quad_input_constructor);
    add_output_constructor(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, pcm_quad_output_constructor);
}

pcm_real_input_t::pcm_real_input_t(const string_t& name_in, node_t& parent_in)
: pcm_input_t(JACKALOPE_PCM_CHANNEL_CLASS_REAL, name_in, parent_in)
{ }

pcm_quad_input_t::pcm_quad_input_t(const string_t& name_in, node_t& parent_in)
: pcm_input_t(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, name_in, parent_in)
{ }

pcm_real_output_t::pcm_real_output_t(const string_t& name_in, node_t& parent_in)
: pcm_output_t(JACKALOPE_PCM_CHANNEL_CLASS_REAL, name_in, parent_in)
{ }

pcm_quad_output_t::pcm_quad_output_t(const string_t& name_in, node_t& parent_in)
: pcm_output_t(JACKALOPE_PCM_CHANNEL_CLASS_QUAD, name_in, parent_in)
{ }

} // namespace jackalope
