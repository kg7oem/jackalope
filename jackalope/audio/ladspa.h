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

#include <jackalope/audio.h>
#include <jackalope/types.h>

#define JACKALOPE_AUDIO_LADSPA_PATH_ENV "LADSPA_PATH"
#define JACKALOPE_AUDIO_LADSPA_PATH_DEFAULT "/usr/lib/ladspa"
#define JACKALOPE_AUDIO_LADSPA_CLASS "audio::ladspa"
#define JACKALOPE_AUDIO_LADSPA_PROPERTY_TYPE "plugin:type"
#define JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE "plugin:file"

namespace jackalope {

namespace audio {

extern "C" {
#include "ext/ladspa.h"
}

void ladspa_init();

using ladspa_data_t = LADSPA_Data;
using ladspa_descriptor_t = LADSPA_Descriptor;
using ladspa_handle_t = LADSPA_Handle;
using ladspa_id_t = size_t;
using ladspa_port_descriptor_t = LADSPA_PortDescriptor;

struct ladspa_node_t : public audio_node_t {
    ladspa_node_t(const string_t& node_name_in);
    virtual void init() override;
};

} // namespace audio

} // namespace jackalope
