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

#include <jackalope/async.h>
#include <jackalope/graph.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

node_t::node_t(const init_args_t init_args_in)
: object_t(init_args_in), name(init_args_get(JACKALOPE_PROPERTY_NODE_NAME, init_args))
{
    assert(name != "");
}

void node_t::set_undef_property(const string_t& name_in)
{
    assert_lockable_owner();

    auto property = get_property(name_in);

    if (property->is_defined()) {
        return;
    }

    auto graph = get_graph();

    if (! graph->has_property(name_in)) {
        return;
    }

    auto graph_property = graph->get_property(name_in);

    if (graph_property->is_defined()) {
        property->set(graph_property->get());
    }
}

shared_t<graph_t> node_t::get_graph()
{
    assert_lockable_owner();

    auto shared = graph.lock();

    assert(shared != nullptr);

    return shared;
}

void node_t::set_graph(shared_t<graph_t> graph_in)
{
    assert_lockable_owner();

    assert(graph_in != nullptr);

    graph = graph_in;
}

void node_t::activate()
{
    assert_lockable_owner();

    if (graph.expired()) {
        throw_runtime_error("node graph weak pointer was expired when activating node");
    }

    add_property(JACKALOPE_PROPERTY_NODE_NAME, property_t::type_t::string, init_args);

    object_t::activate();
}

void node_t::start()
{
    assert_lockable_owner();

    NODE_LOG(info, "Starting node");

    object_t::start();

    NODE_LOG(info, "Done starting node");

    run_if_needed();
}

void node_t::stop()
{
    assert_lockable_owner();

    object_t::stop();
}

void node_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    assert_lockable_owner();

    assert(activated_flag);

    object_t::deliver_one_message(message_in);

    if (started_flag) {
        run_if_needed();
    }
}

void node_t::run_if_needed()
{
    assert_lockable_owner();

    assert(started_flag);

    while(1) {
        if (stopped_flag || ! should_run()) {
            break;
        }

        run();
    }
}

} //namespace jackalope
