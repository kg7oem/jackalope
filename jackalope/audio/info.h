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
#include <jackalope/module.h>
#include <jackalope/types.h>

namespace jackalope {

module_info_t * audio_module_info_constructor();

class audio_module_info_t : public module_info_t {

public:
    audio_module_info_t() = default;
    virtual ~audio_module_info_t() = default;

    inline static const string_t name = "jackalope::audio";
    static const pool_map_t<string_t, channel_info_t *> channel_info;
    static const pool_map_t<string_t, plugin_constructor_t> plugin_constructors;

    virtual const string_t& get_name() override;
    virtual const pool_map_t<string_t, channel_info_t *>& get_channel_info() override;
    virtual const pool_map_t<string_t, plugin_constructor_t>& get_plugin_constructors() override;
};

} //namespace jackalope
