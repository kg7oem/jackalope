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

#include <cstdlib>

#include <jackalope/exception.h>
#include <jackalope/audio/ladspa.h>

namespace jackalope {

namespace audio {

static string_t ladspa_path;

static ladspa_node_t * ladspa_node_constructor(const string_t& node_name_in)
{
    return new ladspa_node_t(node_name_in);
}

void ladspa_init()
{
    auto from_env = std::getenv(JACKALOPE_AUDIO_LADSPA_PATH_ENV);

    if (from_env == nullptr) {
        ladspa_path = JACKALOPE_AUDIO_LADSPA_PATH_DEFAULT;
    } else {
        ladspa_path = from_env;
    }

    add_audio_node_constructor(JACKALOPE_AUDIO_LADSPA_CLASS, ladspa_node_constructor);
}

ladspa_node_t::ladspa_node_t(const string_t& node_name_in)
: audio_node_t(node_name_in, JACKALOPE_AUDIO_LADSPA_CLASS)
{
    add_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE, property_t::type_t::string);
    add_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_TYPE, property_t::type_t::size);
}

void ladspa_node_t::init()
{
    auto type_is_defined = get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_TYPE).is_defined();
    auto file_is_defined = get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE).is_defined();

    if(! type_is_defined && ! file_is_defined) {
        throw_runtime_error("no LADSPA type and no filename was specified");
    } else if(! file_is_defined) {
        throw_runtime_error("can't search for a LADSPA plugin by type yet");
    } else if (! type_is_defined) {
        throw_runtime_error("can't deduce LADSPA type using only file yet");
    }

    audio_node_t::init();
}

} // namespace audio

} // namespace jackalope
