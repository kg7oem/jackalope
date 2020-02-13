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

#include <jackalope/audio/channel.h>
#include <jackalope/property.h>

namespace jackalope {

const prop_args_t audio_channel_properties = {
    { JACKALOPE_PROPERTY_AUDIO_BUFFER_SIZE, property_t::type_t::size },
    { JACKALOPE_PROPERTY_AUDIO_SAMPLE_RATE, property_t::type_t::size },
};

} //namespace jackalope
