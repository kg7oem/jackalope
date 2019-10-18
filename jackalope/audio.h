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

#include <initializer_list>

#include <jackalope/node.h>
#include <jackalope/pcm.h>
#include <jackalope/string.h>

#define JACKALOPE_AUDIO_DOMAIN_CLASS_NAME    "audio::domain"
#define JACKALOPE_AUDIO_NODE_CLASS_PREFIX    "audio::node::"
#define JACKALOPE_AUDIO_DRIVER_CLASS_PREFIX  "audio::driver::"
#define JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE "audio:sample_rate"
#define JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE "audio:buffer_size"

namespace jackalope {

class audio_node_t;
class audio_domain_t;
class audio_driver_t;

void audio_init();

class audio_node_t : public node_t {
    audio_domain_t * domain = nullptr;

public:
    // audio_node_t(const string_t& name_in, const string_t& class_name_in, node_init_list_t init_list_in = node_init_list_t());
    audio_node_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual void set_domain(audio_domain_t * domain_in);
    virtual audio_domain_t& get_domain();
    virtual void activate() override;
    virtual void input_ready(input_t& input_in) override;
    virtual void pcm_ready();
    virtual void notify() override;
};

class audio_domain_t : public node_t {

protected:
    pool_list_t<audio_node_t *> audio_nodes;
    pcm_buffer_t<real_t> zero_buffer;
    audio_driver_t * driver = nullptr;

public:
    audio_domain_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~audio_domain_t();
    virtual size_t get_sample_rate();
    virtual size_t get_buffer_size();
    virtual real_t * get_zero_buffer_pointer();
    virtual void activate() override;
    virtual void reset();
    virtual audio_node_t * _make_audio_node(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());

    template <class T = audio_node_t>
    T * make_node(node_init_list_t init_list_in)
    {
        auto new_node = node_t::make<T>(init_list_in);
        new_node->set_domain(this);
        audio_nodes.push_back(new_node);
        return new_node;
    }

    template <class T = audio_node_t, typename... Args>
    T * make_audio_node(Args... args)
    {
        auto new_node = _make_audio_node(args...);
        return dynamic_cast<T *>(new_node);
    }

    virtual audio_driver_t * _make_audio_driver(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());

    template <class T = audio_driver_t, typename... Args>
    T * make_audio_driver(Args... args)
    {
        auto new_node = _make_audio_driver(args...);
        return dynamic_cast<T *>(new_node);
    }

    virtual void input_ready(input_t& input_in) override;
    virtual void pcm_ready();
    virtual void process(real_t ** source_buffer_in, real_t ** sink_buffer_in);
    virtual void process(const real_t * source_buffer_in, real_t * sink_buffer_in);
    virtual void notify() override;
};

struct audio_driver_t : public node_t {
    audio_domain_t * domain = nullptr;

    audio_driver_t(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~audio_driver_t() = default;
    virtual void set_domain(audio_domain_t * domain_in);
};

} // namespace jackalope
