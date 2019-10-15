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

#include <jackalope/audio.h>
#include <jackalope/logging.h>

namespace jackalope {

audio_node_t::audio_node_t(const string_t& name_in)
: node_t(name_in)
{ }

void audio_node_t::input_ready(input_t& input_in)
{
    log_info("Input is ready: ", input_in.get_name());
}

} // namespace jackalope
