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
#include <jackalope/network.forward.h>
#include <jackalope/node.forward.h>
#include <jackalope/object.h>
#include <jackalope/types.h>

#define JACKALOPE_PROPERTY_NODE_NAME "node.name"

namespace jackalope {

class node_t : public object_t {

protected:
    bool activated_flag = false;
    weak_t<graph_t> graph;
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;

    node_t(const string_t& type_in, const init_args_t& init_args_in);
    node_t(const init_args_t& init_args_in);
    virtual void deliver_one_message(shared_t<abstract_message_t> message_in) override;
    virtual void message_link_available(shared_t<link_t> link_in);
    virtual void message_link_ready(shared_t<link_t> link_in);
    virtual void message_sink_ready(shared_t<sink_t> sink_in);
    virtual void sink_ready(shared_t<sink_t> sink_in);
    virtual void message_source_available(shared_t<source_t> source_in);
    virtual void source_available(shared_t<source_t> source_in);

public:
    const string_t name;

    virtual string_t description() override;

    shared_t<graph_t> get_graph();
    void set_graph(shared_t<graph_t> graph_in);
    virtual void set_undef_property(const string_t& name_in);

    virtual bool is_activated();

    virtual size_t get_num_sources();
    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in);
    virtual shared_t<source_t> _get_source(const string_t& source_name_in);
    virtual shared_t<source_t> _get_source(const size_t source_num_in);

    template <class T = source_t, typename... Args>
    shared_t<T> get_source(Args... args)
    {
        return dynamic_pointer_cast<T>(_get_source(args...));
    }

    virtual size_t get_num_sinks();
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in);
    virtual shared_t<sink_t> _get_sink(const string_t& sink_name_in);
    virtual shared_t<sink_t> _get_sink(const size_t sink_num_in);
    virtual shared_t<sink_t> _get_forward_sink(const string_t& source_name_in);
    virtual shared_t<source_t> _get_forward_source(const string_t& sink_name_in);

    template <class T = sink_t, typename... Args>
    shared_t<T> get_sink(Args... args)
    {
        return dynamic_pointer_cast<T>(_get_sink(args...));
    }

    template<class T = sink_t, typename... Args>
    shared_t<T> get_forward_sink(Args... args)
    {
        return dynamic_pointer_cast<T>(_get_forward_sink(args...));
    }

    template<class T = source_t, typename... Args>
    shared_t<T> get_forward_source(Args... args)
    {
        return dynamic_pointer_cast<T>(_get_forward_source(args...));
    }

    virtual void link(const string_t& source_name_in, shared_t<node_t> target_node_in, const string_t& target_sink_name_in);
    virtual void forward(const string_t& source_name_in, shared_t<node_t> target_network_in, const string_t& target_source_name_in);
    virtual void init() override;
    virtual void activate();
    virtual void start() override;
};

} //namespace jackalope
