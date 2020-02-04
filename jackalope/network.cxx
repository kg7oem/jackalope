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

#include <jackalope/network.h>

namespace jackalope {

static shared_t<network_t> network_node_constructor(const string_t& type_in, const init_args_t& init_args_in)
{
    assert(type_in == JACKALOPE_OBJECT_TYPE_NETWORK);

    return network_t::make(init_args_in);
}

void network_init()
{
    add_object_constructor(JACKALOPE_OBJECT_TYPE_NETWORK, network_node_constructor);
}

shared_t<network_t> network_t::make(const init_args_t& init_args_in)
{
    auto new_network = jackalope::make_shared<network_t>(JACKALOPE_OBJECT_TYPE_NETWORK, init_args_in);

    auto lock = new_network->get_object_lock();
    new_network->init();

    return new_network;
}

network_t::network_t(const string_t& type_in, const init_args_t& init_args_in)
: node_t(type_in, init_args_in)
{ }

void network_t::activate()
{
    assert_lockable_owner();

    network_graph = graph_t::make(get_graph()->init_args);
    auto network_graph_lock = network_graph->get_object_lock();

    network_graph->connect(JACKALOPE_SIGNAL_OBJECT_STOPPED, shared_obj(), JACKALOPE_SLOT_OBJECT_STOP);

    node_t::activate();
}

void network_t::start()
{
    assert_lockable_owner();

    auto graph_lock = network_graph->get_object_lock();
    network_graph->start();
}

shared_t<node_t> network_t::make_node(const init_args_t& init_args_in)
{
    assert_lockable_owner();

    auto graph_lock = network_graph->get_object_lock();
    return network_graph->make_node(init_args_in);
}

shared_t<source_t> network_t::add_source(const string_t& source_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto forward_sink = sink_t::make(source_name_in, type_in, shared_obj());
    auto new_source = node_t::add_source(source_name_in, type_in);

    new_source->link(forward_sink);
    source_forward_sinks[source_name_in] = forward_sink;

    return new_source;
}

shared_t<sink_t> network_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto forward_source = source_t::make(sink_name_in, type_in, shared_obj());
    auto new_sink = node_t::add_sink(sink_name_in, type_in);

    forward_source->link(new_sink);
    sink_forward_sources[sink_name_in] = forward_source;

    return new_sink;
}

} //namespace jackalope
