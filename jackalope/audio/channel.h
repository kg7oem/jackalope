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

#include <jackalope/property.h>
#include <jackalope/types.h>

#define JACKALOPE_CHANNEL_TYPE_AUDIO "audio"
#define JACKALOPE_PROPERTY_AUDIO_BUFFER_SIZE "audio.buffer_size"
#define JACKALOPE_PROPERTY_AUDIO_SAMPLE_RATE "audio.sample_rate"

namespace jackalope {

extern const prop_args_t audio_channel_properties;

} //namespace jackalope
