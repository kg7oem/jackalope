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

#include <jackalope/forward.h>
#include <jackalope/plugin.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

using module_info_constructor = function_t<module_info_t * ()>;

void module_init();
void module_load(module_info_t * info_in);

class module_info_t {

public:
    module_info_t() = default;
    virtual ~module_info_t() = default;

    virtual const string_t& get_name();
    virtual const pool_map_t<string_t, channel_info_t *>& get_channel_info();
    virtual const pool_map_t<string_t, plugin_constructor_t>& get_plugin_constructors();
};

} //namespace jackalope
