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

namespace jackalope {

static input_t * input_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_input_t(name_in, parent_in);
}

static output_t * output_constructor(const string_t& name_in, node_t& parent_in)
{
    return new pcm_output_t(name_in, parent_in);
}

void pcm_init()
{
    add_input_constructor(JACKALOPE_PCM_CHANNEL_CLASS, input_constructor);
    add_output_constructor(JACKALOPE_PCM_CHANNEL_CLASS, output_constructor);
}

pcm_input_t::pcm_input_t(const string_t& name_in, node_t& parent_in)
: input_t(name_in, parent_in)
{ }

void pcm_input_t::link(output_t& output_in) noexcept
{
    auto new_link = new link_t(output_in, *this);

    output_in.add_link(new_link);
    add_link(new_link);
}

pcm_output_t::pcm_output_t(const string_t& name_in, node_t& parent_in)
: output_t(name_in, parent_in)
{ }

} // namespace jackalope
