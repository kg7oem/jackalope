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
#include <jackalope/graph.forward.h>
#include <jackalope/node.forward.h>
#include <jackalope/object.h>
#include <jackalope/types.h>

#define JACKALOPE_PROPERTY_NODE_NAME "node.name"

#define NODE_LOG(level, ...) JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::level_t::level,  "node(", this->name, "): ", __VA_ARGS__)

namespace jackalope {

struct link_available_message_t : public message_t<shared_t<link_t>> {
    static const string_t message_name;
    link_available_message_t(shared_t<link_t> link_in);
};

struct link_ready_message_t : public message_t<shared_t<link_t>> {
    static const string_t message_name;
    link_ready_message_t(shared_t<link_t> link_in);
};

class node_t : public object_t {

protected:
    bool activated_flag = false;
    weak_t<graph_t> graph;
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;

    node_t(const init_args_t init_args_in);
    virtual void message_link_available(shared_t<link_t> link_in);
    virtual void message_link_ready(shared_t<link_t> link_in);

public:
    const string_t name;

    virtual ~node_t();
    shared_t<graph_t> get_graph();
    void set_graph(shared_t<graph_t> graph_in);
    virtual void set_undef_property(const string_t& name_in);
    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const size_t source_num_in);
    virtual size_t get_num_sources();
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& sink_name_in);
    virtual shared_t<sink_t> get_sink(const size_t sink_num_in);
    virtual size_t get_num_sinks();
    virtual void link(const string_t& source_name_in, shared_t<node_t> target_node_in, const string_t& target_sink_name_in);
    virtual void init() override;
    virtual void activate();
    virtual void start() override;
};

} //namespace jackalope
