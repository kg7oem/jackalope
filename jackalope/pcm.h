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

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/node.h>
#include <jackalope/pcm.tools.h>
#include <jackalope/types.h>

#define JACKALOPE_CHANNEL_TYPE_PCM_REAL      "pcm[real]"
#define JACKALOPE_CHANNEL_TYPE_PCM_QUAD      "pcm[quad]"
#define JACKALOPE_PROPERTY_PCM_BUFFER_SIZE   "pcm.buffer_size"
#define JACKALOPE_PROPERTY_PCM_SAMPLE_RATE   "pcm.sample_rate"
#define JACKALOPE_SIGNAL_PCM_READY           "pcm.ready"

namespace jackalope {

void pcm_init();

class pcm_node_t : public node_t {

protected:
    virtual void pcm_ready();

public:
    pcm_node_t(const init_list_t& init_list_in);
    virtual void init() override;
    virtual void activate() override;
};

template <typename T>
class pcm_buffer_t : public base_t {

protected:
    T * data = nullptr;

public:
    const size_t num_samples;

    pcm_buffer_t(const size_t num_samples_in)
    : num_samples(num_samples_in)
    {
        assert(data == nullptr);

        if (num_samples == 0) {
            throw_runtime_error("number of samples must be greater than 0");
        }

        data = new T[num_samples];
        pcm_zero(data, num_samples);
    }

    virtual ~pcm_buffer_t()
    {
        if (data != nullptr) {
            delete data;
            data = nullptr;
        }
    }

    T * get_pointer()
    {
        assert(data != nullptr);
        return data;
    }
};

template <typename T>
class pcm_source_t : public source_t {

public:
    pcm_source_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
    : source_t(name_in, type_in, parent_in)
    { }

    // virtual void activate() override
    // {
    //     auto& buffer_size_prop = get_parent()->get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

    //     buffer.set_num_samples(buffer_size_prop->get_size());

    //     source_t::activate();
    // }

    // pcm_buffer_t<T>& get_buffer()
    // {
    //     return buffer;
    // }
};

template <typename T>
class pcm_sink_t : public sink_t {

public:
    pcm_sink_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
    : sink_t(name_in, type_in, parent_in)
    { }

    // virtual void activate() override
    // {
    //     auto& buffer_size_prop = get_parent()->get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

    //     buffer.set_num_samples(buffer_size_prop->get_size());

    //     sink_t::activate();
    // }

    // pcm_buffer_t<T>& get_buffer()
    // {
    //     return buffer;
    // }
};

} // namespace jackalope
