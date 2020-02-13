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

#include <jackalope/plugin.h>

namespace jackalope {

plugin_t::plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: node_t(project_in, init_args_in)
{ }

filter_plugin_t::filter_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: plugin_t(project_in, init_args_in)
{ }

} //namespace jackalope
