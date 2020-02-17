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
#include <jackalope/audio/plugins.h>

namespace jackalope {

shared_t<audio_gain_plugin_t> audio_gain_plugin_t::make(shared_t<project_t> project_in, const init_args_t& init_args_in)
{
    return object_t::make<audio_gain_plugin_t>(project_in, init_args_in);
}

audio_gain_plugin_t::audio_gain_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: filter_plugin_t(project_in, init_args_in)
{ }

void audio_gain_plugin_t::will_init()
{
    assert_lockable_owner();

    add_channel_type(audio_channel_info_t::type);

    filter_plugin_t::will_init();
}

const string_t& audio_gain_plugin_t::get_type()
{
    return type;
}

void audio_gain_plugin_t::execute()
{
    throw_runtime_error("can't execute yet");
}

} //namespace jackalope
