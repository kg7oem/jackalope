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

#include <jackalope/audio.h>
#include <jackalope/types.h>

#define JACKALOPE_PCM_LADSPA_PATH_ENV "LADSPA_PATH"
#define JACKALOPE_PCM_LADSPA_PATH_DEFAULT "/usr/lib/ladspa"
#define JACKALOPE_PCM_LADSPA_CLASS "pcm::node::ladspa"
#define JACKALOPE_PCM_LADSPA_PROPERTY_ID "plugin:id"
#define JACKALOPE_PCM_LADSPA_PROPERTY_FILE "plugin:file"

namespace jackalope {

namespace pcm {

extern "C" {
#include "ext/ladspa.h"
}

void ladspa_init();

using ladspa_data_t = LADSPA_Data;
using ladspa_descriptor_t = LADSPA_Descriptor;
using ladspa_handle_t = LADSPA_Handle;
using ladspa_id_t = size_t;
using ladspa_port_descriptor_t = LADSPA_PortDescriptor;
using ladspa_descriptor_function_t = LADSPA_Descriptor_Function;

struct ladspa_file_t : public baseobj_t {
    ladspa_descriptor_function_t descriptor_fn = nullptr;
    pool_map_t<ladspa_id_t, const ladspa_descriptor_t *> id_to_descriptor;
    void * handle = nullptr;

    public:
    const string_t path;

    ladspa_file_t(const string_t& path_in);
    ~ladspa_file_t();
    const pool_vector_t<const ladspa_descriptor_t *> get_descriptors();
    const ladspa_descriptor_t * get_descriptor(const ladspa_id_t id_in);
};

struct ladspa_instance_t : public baseobj_t {
    ladspa_file_t& file;
    const ladspa_id_t id;
    const ladspa_descriptor_t * descriptor = nullptr;
    ladspa_handle_t handle = nullptr;
    pool_map_t<string_t, size_t> port_name_to_num;

    ladspa_instance_t(ladspa_file_t& file_in, const ladspa_id_t id_in);
    ~ladspa_instance_t();
    size_t get_num_ports();
    ladspa_port_descriptor_t get_port_descriptor(const size_t port_num_in);
    const string_t get_port_name(const size_t port_num_in);
    size_t get_port_num(const string_t& port_name_in);
    ladspa_data_t get_port_default(const size_t port_num_in);
    void instantiate(const size_t sample_rate_in);
    void activate();
    void run(const size_t num_samples_in);
    void connect_port(const size_t port_num_in, ladspa_data_t * pointer_in);
};

struct ladspa_node_t : public pcm_node_t {
    const string_t class_name = JACKALOPE_PCM_LADSPA_CLASS;

    ladspa_file_t * file = nullptr;
    ladspa_instance_t * instance = nullptr;

    ladspa_node_t(const string_t& node_name_in, node_init_list_t init_list_in = node_init_list_t());
    ~ladspa_node_t();
    virtual void init() override;
    virtual void init_file();
    virtual void init_instance();
    virtual void activate() override;
    virtual void pcm_ready() override;
};

} // namespace pcm

} // namespace jackalope
