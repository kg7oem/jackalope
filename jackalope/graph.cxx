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

shared_t<graph_t> graph_t::make(const init_args_t* init_args_in)
{
    return make(*init_args_in);
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

graph_t::graph_t(const init_args_t * init_args_in)
: object_t(JACKALOPE_TYPE_GRAPH, init_args_in)
{
    for(auto i : *init_args) {
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

void graph_t::add_node(shared_t<node_t> node_in)
{
    assert_lockable_owner();
    assert_object_owner(node_in);

    assert(init_flag);

    auto shared_this = shared_obj<graph_t>();

    if (nodes.find(node_in->name) != nodes.end()) {
        throw_runtime_error("Can not add node with duplicate name to graph: ", node_in->name);
    }

    if (node_in->get_graph() != shared_this) {
        throw_runtime_error("Can not add an activated node to a graph if the node's graph is not us");
    }

    if (! node_in->is_activated()) {
        bool activate_flag = true;

        if (init_args_has("node.activate", node_in->init_args)) {
            string_t should_activate = init_args_get("node.activate", node_in->init_args);

            if (should_activate != "true") {
                activate_flag = false;
            }
        }

        if (activate_flag) {
            node_in->activate();
        }
    }

    nodes[node_in->name] = node_in;
}

shared_t<node_t> graph_t::make_node(const init_args_t& init_args_in)
{
    assert_lockable_owner();
    assert(init_flag);

    auto new_node = object_t::make<node_t>(init_args_in);
    auto new_node_lock = new_node->get_object_lock();
    new_node->set_graph(shared_obj<graph_t>());

    add_node(new_node);

    return new_node;
}

shared_t<network_t> graph_t::make_network(const init_args_t& init_args_in)
{
    assert_lockable_owner();

    assert(init_flag);

    auto new_network = network_t::make(init_args_in);
    auto new_network_lock = new_network->get_object_lock();

    new_network->set_graph(shared_obj<graph_t>());
    new_network->activate();

    add_node(new_network);

    return new_network;
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

    object_t::start();

    for(auto i : nodes) {
        auto node = i.second;
        auto lock = node->get_object_lock();
        node->start();
    }
}

void graph_t::stop()
{
    assert_lockable_owner();

    assert(started_flag);
    assert(! stopped_flag);

    for(auto i : nodes) {
        auto node = i.second;
        auto lock = node->get_object_lock();

        if (! node->is_stopped()) {
            object_log_info("stopping node: ", node->name);
            node->stop();
        }
    }

    object_t::stop();
}

} // namespace jackalope
