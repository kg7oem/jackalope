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
#include <jackalope/node.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_CHANNEL_CLASS_REAL "pcm[real]"
#define JACKALOPE_PCM_CHANNEL_CLASS_QUAD "pcm[quad]"

namespace jackalope {

template <typename T>
struct pcm_buffer_t : public baseobj_t {
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
        pointer = static_cast<T *>(std::malloc(get_num_bytes()));
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

    size_t get_num_bytes()
    {
        return num_samples * sizeof(sample_t);
    }

    sample_t * get_pointer()
    {
        return pointer;
    }
};

template <typename T>
struct pcm_input_t : public input_t {
    using sample_type = T;

    pcm_input_t(const string_t& class_name_in, const string_t& name_in, node_t& parent_in)
    : input_t(class_name_in, name_in, parent_in)
    { }

    virtual ~pcm_input_t() = default;

    virtual void output_ready(output_t& output_in) override
    {
        log_info("Output is ready: ", output_in.get_name());

        for (auto i : links) {
            if (! i->is_enabled()) {
                continue;
            } else if (! i->from.is_ready()) {
                return;
            }
        }

        notify();
    }

    void notify() override
    {
        assert(ready_flag == false);

        ready_flag = true;

        parent.input_ready(*this);
    }

    void link(output_t& output_in) override
    {
        auto new_link = new link_t(output_in, *this);

        output_in.add_link(new_link);
        add_link(new_link);

        new_link->enable();
    }

    void unlink(link_t * link_in) override
    {
        link_in->from.remove_link(link_in);
        link_in->to.remove_link(link_in);

        delete link_in;
    }
};

struct pcm_real_input_t : public pcm_input_t<real_t> {
    pcm_real_input_t(const string_t& name_in, node_t& parent_in);
    virtual ~pcm_real_input_t() = default;
};

struct pcm_quad_input_t : public pcm_input_t<complex_t> {
    pcm_quad_input_t(const string_t& name_in, node_t& parent_in);
    virtual ~pcm_quad_input_t() = default;
};

template <typename T>
struct pcm_output_t : public output_t {
    using sample_type = T;

    pcm_buffer_t<sample_type> buffer{512};

    pcm_output_t(const string_t& class_name_in, const string_t& name_in, node_t& parent_in)
    : output_t(class_name_in, name_in, parent_in)
    { }

    virtual ~pcm_output_t() = default;

    virtual void link(input_t& input_in) override
    {
        auto new_link = new link_t(*this, input_in);

        input_in.add_link(new_link);
        add_link(new_link);

        new_link->enable();
    }

    virtual void unlink(link_t * link_in) override
    {
        link_in->from.remove_link(link_in);
        link_in->to.remove_link(link_in);

        delete link_in;
    }

    virtual void notify() override
    {
        for (auto i : links) {
            if (! i->is_enabled()) {
                continue;
            }

            i->to.output_ready(*this);
        }
    }
};

struct pcm_real_output_t : public pcm_output_t<real_t> {
    pcm_real_output_t(const string_t& name_in, node_t& parent_in);
    virtual ~pcm_real_output_t() = default;
};

struct pcm_quad_output_t : public pcm_output_t<complex_t> {
    pcm_buffer_t<quad_t> buffer{512};

    pcm_quad_output_t(const string_t& name_in, node_t& parent_in);
    virtual ~pcm_quad_output_t() = default;
};

void pcm_init();

} // namespace jackalope
