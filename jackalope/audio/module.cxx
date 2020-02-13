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
#include <jackalope/exception.h>
#include <jackalope/audio/gain.h>
#include <jackalope/audio/module.h>

namespace jackalope {

const string_t audio_module_info_t::name = "jackalope::audio";

const pool_map_t<string_t, plugin_constructor_t> audio_module_info_t::plugin_constructors = {
    { audio_gain_plugin_t::type, audio_gain_plugin_t::make },
};

const pool_map_t<string_t, prop_args_t> audio_module_info_t::channel_properties = {
    { JACKALOPE_CHANNEL_TYPE_AUDIO,  audio_channel_properties },
};

const pool_map_t<string_t, sink_constructor_t> audio_module_info_t::sink_constructors;
const pool_map_t<string_t, source_constructor_t> audio_module_info_t::source_constructors;

module_info_t * audio_module_info_constructor()
{
    return new audio_module_info_t();
}

const pool_map_t<string_t, plugin_constructor_t>& audio_module_info_t::get_plugin_constructors()
{
    return plugin_constructors;
}

const pool_map_t<string_t, sink_constructor_t>& audio_module_info_t::get_sink_constructors()
{
    return sink_constructors;
}

const pool_map_t<string_t, source_constructor_t>& audio_module_info_t::get_source_constructors()
{
    return source_constructors;
}

const string_t& audio_module_info_t::get_name()
{
    return name;
}

} //namespace jackalope
