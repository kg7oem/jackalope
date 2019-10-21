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

#define JACKALOPE_PCM_CHANNEL_CLASS         "pcm"
#define JACKALOPE_PCM_CHANNEL_CLASS_REAL    "pcm[real]"
#define JACKALOPE_PCM_CHANNEL_CLASS_QUAD    "pcm[quad]"
#define JACKALOPE_PCM_DOMAIN_CLASS_NAME     "pcm::domain"
#define JACKALOPE_PCM_DRIVER_CLASS_PREFIX   "pcm::driver::"
#define JACKALOPE_PCM_NODE_CLASS_PREFIX     "pcm::node::"
#define JACKALOPE_PCM_PROPERTY_BUFFER_SIZE  "pcm:buffer_size"
#define JACKALOPE_PCM_PROPERTY_SAMPLE_RATE  "pcm:sample_rate"

namespace jackalope {

class pcm_node_t;
class pcm_domain_t;
struct pcm_driver_t;

void pcm_init();

template <class T = pcm_domain_t>
shared_t<T> make_pcm_domain(node_init_list_t init_list_in)
{
    return node_t::make<T>(init_list_in);
}

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

class pcm_node_t : public node_t {
    shared_t<pcm_domain_t> domain = nullptr;

public:
    pcm_node_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual void set_domain(shared_t<pcm_domain_t> domain_in);
    virtual pcm_domain_t& get_domain();
    virtual void activate() override;
    virtual void input_ready(shared_t<input_t> input_in) override;
    virtual void pcm_ready();
    virtual void notify() override;
};

class pcm_domain_t : public node_t {

protected:
    pool_list_t<weak_t<pcm_node_t>> pcm_nodes;
    pcm_buffer_t<real_t> zero_buffer;
    pcm_driver_t * driver = nullptr;

public:
    pcm_domain_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~pcm_domain_t();
    virtual size_t get_sample_rate();
    virtual size_t get_buffer_size();
    virtual real_t * get_zero_buffer_pointer();
    virtual void activate() override;
    virtual void start() override;
    virtual void reset() override;

    template <class T = pcm_node_t>
    shared_t<T> make_node(node_init_list_t init_list_in)
    {
        auto new_node = node_t::make<T>(init_list_in);

        new_node->set_domain(shared_obj<pcm_domain_t>());
        new_node->activate();

        pcm_nodes.push_back(new_node);

        return new_node;
    }

    template <class T = pcm_driver_t, typename... Args>
    shared_t<T> make_driver(node_init_list_t init_list_in)
    {
        auto new_node = node_t::make<T>(init_list_in);

        new_node->set_domain(shared_obj<pcm_domain_t>());
        new_node->activate();

        return new_node;
    }

    virtual void input_ready(shared_t<input_t> input_in) override;
    virtual void pcm_ready();
    virtual void process(real_t ** source_buffer_in, real_t ** sink_buffer_in);
    virtual void process(const real_t * source_buffer_in, real_t * sink_buffer_in);
    virtual void notify() override;
};

struct pcm_driver_t : public node_t {
    shared_t<pcm_domain_t> domain;

    pcm_driver_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~pcm_driver_t() = default;
    virtual void set_domain(shared_t<pcm_domain_t> domain_in);
    virtual void input_ready(shared_t<input_t> input_in) override;
    virtual void notify() override;
};

} // namespace jackalope
