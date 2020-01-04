// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <thread>

#include <jackalope/graph.h>

namespace jackalope {

shared_t<graph_t> graph_t::make(const init_list_t& init_args_in)
{
    auto graph = jackalope::make_shared<graph_t>(init_args_in);
    auto lock = graph->get_object_lock();

    graph->init();
    graph->activate();

    return graph;
}

graph_t::graph_t(const init_list_t& init_args_in)
: object_t(JACKALOPE_TYPE_GRAPH, init_args_in)
{ }

shared_t<node_t> graph_t::add_node(const init_list_t& init_args_in)
{
    assert_lockable_owner();

    assert(init_flag);

    auto new_node = object_t::make<node_t>(init_args_in);
    auto new_node_lock = new_node->get_object_lock();

    new_node->set_graph(shared_obj<graph_t>());
    new_node->activate();

    nodes.push_back(new_node);

    return new_node;
}

void graph_t::start()
{
    assert_lockable_owner();

    assert(init_flag);
    assert(! started_flag);

    for(auto i : nodes) {
        auto lock = i->get_object_lock();
        i->start();
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
        auto lock = i->get_object_lock();

        if (! i->is_stopped()) {
            log_info("stopping node: ", i->name);
            i->stop();
        }
    }

    object_t::stop();
}

} // namespace jackalope
