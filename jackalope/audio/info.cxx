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
#include <jackalope/audio/info.h>
#include <jackalope/audio/plugins.h>

#ifdef CONFIG_ENABLE_SNDFILE
#include <jackalope/audio/sndfile.h>
#endif

namespace jackalope {

const pool_map_t<string_t, channel_info_t *> audio_module_info_t::channel_info = {
    { "audio", new audio_channel_info_t() },
};

const pool_map_t<string_t, plugin_constructor_t> audio_module_info_t::plugin_constructors = {
    { audio_gain_plugin_t::type, audio_gain_plugin_t::make },

#ifdef CONFIG_ENABLE_SNDFILE
    { audio::sndfile_plugin_t::type, audio::sndfile_plugin_t::make },
#endif

};

module_info_t * audio_module_info_constructor()
{
    return new audio_module_info_t();
}

const string_t& audio_module_info_t::get_name()
{
    return audio_module_info_t::name;
}

const pool_map_t<string_t, channel_info_t *>& audio_module_info_t::get_channel_info()
{
    return channel_info;
}

const pool_map_t<string_t, plugin_constructor_t>& audio_module_info_t::get_plugin_constructors()
{
    return plugin_constructors;
}

} //namespace jackalope
