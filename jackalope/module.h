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
#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

void module_init();
void module_load(module_info_t * info_in);

using module_info_constructor = function_t<module_info_t *>();
using plugin_constructor_t = function_t<shared_t<plugin_t> ()>;
using sink_constructor_t = function_t<shared_t<sink_t>> ();
using source_constructor_t = function_t<shared_t<source_t> ()>;

class module_info_t : base_obj_t {

public:
    virtual const string_t& get_name() = 0;
    virtual const pool_map_t<string_t, plugin_constructor_t>& get_plugin_constructors() = 0;
    virtual const pool_map_t<string_t, sink_constructor_t>& get_sink_constructors() = 0;
    virtual const pool_map_t<string_t, sink_constructor_t>& get_source_constructors() = 0;
};

} //namespace jackalope
