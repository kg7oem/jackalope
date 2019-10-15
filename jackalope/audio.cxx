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
{
    add_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
    add_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
}

void audio_node_t::activate()
{
    if (get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).get_size() == 0) {
        throw_runtime_error("can not activate an audio node with buffer size of 0");
    }

    if (get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).get_size() == 0) {
        throw_runtime_error("can not activate an audio node with a sample rate of 0");
    }

    node_t::activate();
}

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

audio_domain_t::audio_domain_t(const size_t sample_rate_in, const size_t buffer_size_in)
: sample_rate(sample_rate_in), buffer_size(buffer_size_in)
{ }

audio_domain_t::~audio_domain_t()
{
    for(auto i : audio_nodes) {
        delete i;
    }
}

size_t audio_domain_t::get_sample_rate()
{
    return sample_rate;
}

size_t audio_domain_t::get_buffer_size()
{
    return buffer_size;
}

} // namespace jackalope
