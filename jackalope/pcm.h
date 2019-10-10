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

#include <cstdlib>

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_CHANNEL_CLASS "pcm"

namespace jackalope {

struct pcm_input_t : public input_interface_t, public baseobj_t, public lockable_t {
    pcm_input_t(const string_t& name_in, node_t& parent_in);
    virtual node_t& get_parent() noexcept override;
    virtual const string_t& get_name() noexcept override;
};

template <typename T>
struct pcm_buffer_t : public baseobj_t, public lockable_t {
    using sample_t = T;

    bool owns_memory = false;
    sample_t * pointer = nullptr;
    const size_t num_samples;

    pcm_buffer_t(const size_t num_samples_in)
    : num_samples(num_samples_in)
    {
        assert(owns_memory == false);

        if (num_samples_in == 0) {
            throw_runtime_error("num_samples must be positive");
        }

        // FIXME should use a pool allocator
        pointer = std::malloc(get_num_bytes());
        if (pointer == nullptr) {
            throw_runtime_error("could not allocate memory");
        }

        owns_memory = true;
    }

    pcm_buffer_t(const size_t num_samples_in, sample_t * pointer_in)
    : num_samples(num_samples_in)
    {
        assert(owns_memory == false);

        pointer = pointer_in;
    }

    pcm_buffer_t(const size_t num_samples_in, sample_t * pointer_in, const bool owns_memory_in)
    : num_samples(num_samples_in)
    {
        owns_memory = owns_memory_in;
        pointer = pointer_in;
    }

    virtual ~pcm_buffer_t()
    {
        if (owns_memory) {
            assert(pointer != nullptr);

            free(pointer);
            pointer = nullptr;
        }

        owns_memory = false;
    }

    size_t get_num_bytes() noexcept
    {
        return num_samples * sizeof(sample_t);
    }
};

void pcm_init();

} // namespace jackalope
