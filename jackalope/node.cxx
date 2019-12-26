// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/node.h>

namespace jackalope {

node_library_t * node_library = new node_library_t();

void add_node_constructor(const string_t& class_name_in, node_library_t::constructor_t constructor_in)
{
    node_library->add_constructor(class_name_in, constructor_in);
}

shared_t<node_t> node_t::make(const init_list_t& init_list_in)
{
    if (! init_list_has(JACKALOPE_PROPERTY_NODE_CLASS, init_list_in)) {
        throw_runtime_error("missing node class in arguments");
    }

    auto node_class = init_list_get(JACKALOPE_PROPERTY_NODE_CLASS, init_list_in);
    auto node = node_library->make(node_class, init_list_in);
    auto node_lock = node->get_object_lock();

    node->init();

    return node;
}

node_t::node_t(const init_list_t& init_list_in)
: object_t(init_list_in)
{ }

void node_t::set_graph(shared_t<graph_t> graph_in)
{
    assert_lockable_owner();

    graph = graph_in;
}

shared_t<graph_t> node_t::get_graph()
{
    assert_lockable_owner();

    auto strong_graph = graph.lock();

    assert(strong_graph != nullptr);

    return strong_graph;
}

} // namespace jackalope
