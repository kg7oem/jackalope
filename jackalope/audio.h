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

#include <jackalope/node.h>
#include <jackalope/pcm.h>
#include <jackalope/string.h>

#define JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE "audio:sample_rate"
#define JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE "audio:buffer_size"

namespace jackalope {

struct audio_node_t;
struct audio_domain_t;

void audio_init();

using audio_node_constructor_t = function_t<audio_node_t * (const string_t& name_in)>;
void add_audio_node_constructor(const string_t& class_name_in, audio_node_constructor_t constructor_in);
audio_node_t * make_audio_node(const string_t& class_name_in, const string_t& node_name_in);

class audio_node_t : public node_t {
    audio_domain_t * domain = nullptr;

public:
    audio_node_t(const string_t& name_in, const string_t& class_name_in);
    virtual void set_domain(audio_domain_t * domain_in);
    virtual void activate() override;
    virtual void input_ready(input_t& input_in);
    virtual void pcm_ready();
};

class audio_domain_t : public baseobj_t {

protected:
    size_t sample_rate = 0;
    size_t buffer_size = 0;
    pool_list_t<audio_node_t *> audio_nodes;

public:
    audio_domain_t(const size_t sample_rate_in, const size_t buffer_size_in);
    ~audio_domain_t();
    size_t get_sample_rate();
    size_t get_buffer_size();
    audio_node_t& make_node(const string_t& name_in, const string_t& class_name_in);
};

} // namespace jackalope
