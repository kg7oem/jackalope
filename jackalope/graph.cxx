// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.


#include <jackalope/graph.h>
#include <jackalope/jackalope.h>

namespace jackalope {

shared_t<graph_t> graph_t::make(const init_args_t& init_args_in)
{
    auto graph = jackalope::make_shared<graph_t>(init_args_in);
    auto lock = graph->get_object_lock();

    graph->init();

    return graph;
}

shared_t<graph_t> graph_t::make(const prop_args_t& prop_args_in)
{
    auto graph = jackalope::make_shared<graph_t>(prop_args_in);
    auto lock = graph->get_object_lock();

    graph->init();

    return graph;
}

graph_t::graph_t(const init_args_t& init_args_in)
: object_t(JACKALOPE_TYPE_GRAPH, init_args_in)
{
    for(auto i : init_args_in) {
        auto property = add_property(i.first, property_t::type_t::string);
        property->set_string(i.second);
    }
}

graph_t::graph_t(const prop_args_t& prop_args_in)
: object_t(JACKALOPE_TYPE_GRAPH, { })
{
    for(auto i : prop_args_in) {
        add_property(i.first, i.second);
    }
}

// THREAD safe because it only calls safe methods
shared_t<property_t> graph_t::add_property(const string_t& name_in, property_t::type_t type_in)
{
    return object_t::add_property(name_in, type_in);
}

// THREAD safe because it only calls safe methods
shared_t<property_t> graph_t::add_property(const string_t& name_in, property_t::type_t type_in, const init_args_t& init_args_in)
{
    return object_t::add_property(name_in, type_in, init_args_in);
}

shared_t<node_t> graph_t::add_node(const init_args_t& init_args_in)
{
    assert_lockable_owner();

    assert(init_flag);

    auto new_node = object_t::make<node_t>(init_args_in);
    auto new_node_lock = new_node->get_object_lock();

    if (nodes.find(new_node->name) != nodes.end()) {
        throw_runtime_error("Can not add node with duplicate name to graph: ", new_node->name);
    }

    new_node->set_graph(shared_obj<graph_t>());
    new_node->activate();

    nodes[new_node->name] = new_node;

    return new_node;
}

void graph_t::init()
{
    assert_lockable_owner();

    object_t::init();

    get_property(JACKALOPE_PROPERTY_OBJECT_TYPE)->set(JACKALOPE_TYPE_GRAPH);
}

void graph_t::start()
{
    assert_lockable_owner();

    assert(! started_flag);

    for(auto i : nodes) {
        auto node = i.second;
        auto lock = node->get_object_lock();
        node->start();
    }

    object_t::start();
}

void graph_t::stop()
{
    assert_lockable_owner();

    assert(started_flag);
    assert(! stopped_flag);

    log_info("graph is stopping");

    for(auto i : nodes) {
        auto node = i.second;
        auto lock = node->get_object_lock();

        if (! node->is_stopped()) {
            log_info("stopping node: ", node->name);
            node->stop();
        }
    }

    object_t::stop();
}

} // namespace jackalope
