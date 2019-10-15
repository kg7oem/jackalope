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

#include <jackalope/audio.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/pcm.h>

namespace jackalope {

audio_node_t::audio_node_t(const string_t& name_in)
: node_t(name_in)
{ }

void audio_node_t::input_ready(input_t&)
{
    for (auto i : inputs) {
        auto input = i.second;
        auto input_class = extract_channel_class(input->get_class_name());

        if (input_class != JACKALOPE_PCM_CHANNEL_CLASS) {
            continue;
        } else if (! input->is_ready()) {
            return;
        }
    }

    pcm_ready();
}

void audio_node_t::pcm_ready()
{
    log_info("All pcm inputs are ready: ", name);
}

} // namespace jackalope
