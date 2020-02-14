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

#include <jackalope/audio/plugins.h>

namespace jackalope {

shared_t<audio_gain_plugin_t> audio_gain_plugin_t::make(shared_t<project_t> project_in, const init_args_t& init_args_in)
{
    auto gain_plugin = jackalope::make_shared<audio_gain_plugin_t>(project_in, init_args_in);
    guard_object(gain_plugin, { gain_plugin->init(); gain_plugin->activate(); });
    return gain_plugin;
}

audio_gain_plugin_t::audio_gain_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: filter_plugin_t(project_in, init_args_in)
{ }

const string_t& audio_gain_plugin_t::get_type()
{
    return type;
}

} //namespace jackalope
