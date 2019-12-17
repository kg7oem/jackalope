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
#include <jackalope/pcm.tools.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_CHANNEL_TYPE_REAL    "pcm[real]"
#define JACKALOPE_PCM_CHANNEL_TYPE_QUAD    "pcm[quad]"
#define JACKALOPE_PCM_PROPERTY_BUFFER_SIZE  "pcm:buffer_size"
#define JACKALOPE_PCM_PROPERTY_SAMPLE_RATE  "pcm:sample_rate"

namespace jackalope {

void pcm_init();

template <typename T>
class pcm_buffer_t : public base_t {

protected:
    size_t num_samples = 0;
    T * data = nullptr;

public:
    pcm_buffer_t() { }

    pcm_buffer_t(const size_t num_samples_in)
    : num_samples(num_samples_in)
    {
        set_num_samples(num_samples_in);
    }

    virtual ~pcm_buffer_t()
    {
        if (data != nullptr) {
            delete data;
            data = nullptr;
            num_samples = 0;
        }
    }

    void set_num_samples(const size_t num_samples_in)
    {
        if (num_samples_in == 0) {
            throw_runtime_error("number of samples must be greater than 0");
        }

        if (data != nullptr) {
            delete data;
        }

        data = new T[num_samples_in];
        pcm_zero(data, num_samples_in);
    }

    T * get_pointer()
    {
        assert(data != nullptr);
        return data;
    }
};

template <typename T>
class pcm_source_t : public source_t {

protected:
    pcm_buffer_t<T> buffer;

public:
    pcm_source_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
    : source_t(name_in, type_in, parent_in)
    { }

    virtual void activate() override
    {
        auto& buffer_size_prop = get_parent()->get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

        buffer.set_num_samples(buffer_size_prop.get_size());

        source_t::activate();
    }

    pcm_buffer_t<T>& get_buffer()
    {
        return buffer;
    }
};

template <typename T>
class pcm_sink_t : public sink_t {

protected:
    pcm_buffer_t<T> buffer;

public:
    pcm_sink_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
    : sink_t(name_in, type_in, parent_in)
    { }

    virtual void activate() override
    {
        auto& buffer_size_prop = get_parent()->get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

        buffer.set_num_samples(buffer_size_prop.get_size());

        sink_t::activate();
    }

    pcm_buffer_t<T>& get_buffer()
    {
        return buffer;
    }
};

} // namespace jackalope
