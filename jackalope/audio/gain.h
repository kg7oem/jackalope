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

#include <jackalope/plugin.h>
#include <jackalope/types.h>

#define JACKALOPE_AUDIO_GAIN_OBJECT_TYPE     "audio::gain"

namespace jackalope {

namespace audio {

void gain_init();

class gain_node_t : public filter_plugin_t {

public:
    gain_node_t(const init_args_t init_args_in);

    virtual void init() override;
    virtual void activate() override;
    virtual void execute() override;
};

} // namespace audio

} //namespace jackalope
