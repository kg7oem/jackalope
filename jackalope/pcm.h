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
#include <cstring>

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/node.h>
#include <jackalope/pcm.tools.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_CHANNEL_CLASS       "pcm"
#define JACKALOPE_PCM_CHANNEL_CLASS_REAL  "pcm[real]"
#define JACKALOPE_PCM_CHANNEL_CLASS_QUAD  "pcm[quad]"

namespace jackalope {

template <typename T>
struct pcm_buffer_t : public baseobj_t {
    using sample_t = T;

    bool owns_memory = false;
    sample_t * pointer = nullptr;
    size_t num_samples = 0;

    pcm_buffer_t()
    {
        assert(owns_memory == false);
    }

    virtual ~pcm_buffer_t()
    {
        if (owns_memory) {
            assert(pointer != nullptr);

            delete pointer;
            pointer = nullptr;
        }

        owns_memory = false;
    }

    void set_num_samples(const size_t num_samples_in)
    {
        if (pointer != nullptr) {
            std::free(pointer);
        }

        pointer = new sample_t[num_samples_in];
        owns_memory = true;
    }

    size_t get_num_bytes()
    {
        return num_samples * sizeof(sample_t);
    }

    sample_t * get_pointer()
    {
        return pointer;
    }

    void zero()
    {
        pcm_zero(pointer, num_samples);
    }
};

template <typename T>
struct pcm_input_t : public input_t {
    using sample_t = T;

    pcm_input_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in)
    : input_t(class_name_in, name_in, parent_in)
    { }

    virtual ~pcm_input_t() = default;

    virtual void output_ready(shared_t<output_t>) override
    {
        for (auto i : links) {
            if (! i->get_from()->is_ready()) {
                return;
            }
        }

        notify();
    }

    virtual sample_t * get_buffer_pointer() = 0;

    virtual void link(shared_t<output_t> output_in) override
    {
        auto new_link = jackalope::make_shared<link_t>(output_in, shared_obj());

        output_in->add_link(new_link);
        add_link(new_link);
    }

    virtual void unlink(shared_t<link_t> link_in) override
    {
        link_in->get_from()->remove_link(link_in);
        link_in->get_to()->remove_link(link_in);
    }
};

struct pcm_real_input_t : public pcm_input_t<real_t> {
    pcm_real_input_t(const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~pcm_real_input_t() = default;
    virtual real_t * get_buffer_pointer() override;
};

struct pcm_quad_input_t : public pcm_input_t<complex_t> {
    pcm_quad_input_t(const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~pcm_quad_input_t() = default;
    virtual complex_t * get_buffer_pointer() override;
};

template <typename T>
struct pcm_output_t : public output_t {
    using sample_t = T;

    pcm_buffer_t<sample_t> buffer;

    pcm_output_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in)
    : output_t(class_name_in, name_in, parent_in)
    { }

    virtual ~pcm_output_t() = default;

    virtual void set_num_samples(const size_t num_samples_in)
    {
        buffer.set_num_samples(num_samples_in);
    }

    sample_t * get_buffer_pointer()
    {
        auto pointer = buffer.get_pointer();

        assert(pointer != nullptr);
        return pointer;
    }

    virtual void link(shared_t<input_t> input_in) override
    {
        auto new_link = jackalope::make_shared<link_t>(shared_obj(), input_in);

        input_in->add_link(new_link);
        add_link(new_link);
    }

    virtual void unlink(shared_t<link_t> link_in) override
    {
        link_in->get_from()->remove_link(link_in);
        link_in->get_to()->remove_link(link_in);
    }

    virtual void zero_buffer()
    {
        buffer.zero();
    }
};

struct pcm_real_output_t: public pcm_output_t<real_t> {
    pcm_real_output_t(const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~pcm_real_output_t() = default;
};

struct pcm_quad_output_t: public pcm_output_t<complex_t> {
    pcm_quad_output_t(const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~pcm_quad_output_t() = default;
};

void pcm_init();

} // namespace jackalope
