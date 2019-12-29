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
#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.tools.h>
#include <jackalope/types.h>

#define JACKALOPE_CHANNEL_TYPE_PCM_REAL      "pcm[real]"
#define JACKALOPE_CHANNEL_TYPE_PCM_QUAD      "pcm[quad]"
#define JACKALOPE_PROPERTY_PCM_BUFFER_SIZE   "pcm.buffer_size"
#define JACKALOPE_PROPERTY_PCM_SAMPLE_RATE   "pcm.sample_rate"
#define JACKALOPE_SIGNAL_PCM_SINKS_READY     "pcm.sinks_ready"
#define JACKALOPE_SIGNAL_PCM_SOURCES_READY   "pcm.sources_ready"

namespace jackalope {

class pcm_real_source;
class pcm_real_sink;

void pcm_init();

template <typename T>
class pcm_buffer_t : public base_t {

public:
    using pcm_t = T;

protected:
    pcm_t * data = nullptr;

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

    pcm_t * get_pointer()
    {
        assert(data != nullptr);
        return data;
    }
};

class pcm_node_t : public node_t {

protected:
    virtual void pcm_sinks_ready();
    virtual void pcm_sources_ready();

public:
    pcm_node_t(const init_list_t& init_list_in);
    virtual void init() override;
    virtual void activate() override;
    virtual void reset() override;
    virtual void reset_pcm_sinks();
    virtual void reset_pcm_sources();
};

class pcm_real_sink_t : public sink_t {

public:
    using pcm_t = real_t;
    using buffer_t = pcm_buffer_t<pcm_t>;

    pcm_real_sink_t(const string_t& name_in, const string_t& type_in);
};

class pcm_real_source_t : public source_t {

public:
    using pcm_t = real_t;
    using buffer_t = pcm_buffer_t<pcm_t>;

protected:
    shared_t<buffer_t> buffer;

public:
    pcm_real_source_t(const string_t& name_in, const string_t& type_in);
    virtual void set_buffer(shared_t<buffer_t> buffer_in);
    virtual void notify();
    virtual void link(shared_t<sink_t> sink_in);
};

} // namespace jackalope
