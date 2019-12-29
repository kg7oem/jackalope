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

#include <jackalope/async.h>
#include <jackalope/channel.h>
#include <jackalope/foreign.h>
#include <jackalope/graph.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

shared_t<foreign_graph_t> make_graph(const init_list_t& init_list_in)
{
    auto graph = graph_t::make(init_list_in);
    return jackalope::make_shared<foreign_graph_t>(graph);
}

foreign_node_t::foreign_node_t(shared_t<node_t> node_in)
: node(node_in)
{
    assert(node != nullptr);
}

void foreign_node_t::connect(const string_t& , shared_t<foreign_node_t> , const string_t& )
{ }

void foreign_node_t::connect(const string_t& signal_name_in, shared_t<foreign_graph_t> target_graph_in, const string_t& target_slot_in)
{
    wait_job<void>([&] {
        auto our_lock = node->get_object_lock();
        auto target_lock = target_graph_in->graph->get_object_lock();

        auto signal = node->get_signal(signal_name_in);
        auto slot = target_graph_in->graph->get_slot(target_slot_in);

        signal->subscribe(slot);
    });
}

void foreign_node_t::link(const string_t& source_name_in, shared_t<foreign_node_t> sink_node_in, const string_t& sink_name_in)
{
    wait_job<void>([&] {
        auto source_node = node;
        auto source_node_lock = node->get_object_lock();
        auto sink_node = sink_node_in->node;
        auto sink_node_lock = sink_node->get_object_lock();

        source_node->get_source(source_name_in)->link(sink_node->get_sink(sink_name_in));
    });
}

foreign_graph_t::foreign_graph_t(shared_t<graph_t> graph_in)
: graph(graph_in)
{
    assert(graph != nullptr);
}

void foreign_graph_t::start()
{
    wait_job<void>([this] {
        auto lock = graph->get_object_lock();
        graph->start();
    });
}

shared_t<foreign_node_t> foreign_graph_t::add_node(const init_list_t& init_list_in)
{
    shared_t<node_t> new_node = wait_job<shared_t<node_t>>([&] {
        auto lock = graph->get_object_lock();
        return graph->add_node(init_list_in);
    });

    return jackalope::make_shared<foreign_node_t>(new_node);
}

void foreign_graph_t::wait_signal(const string_t& signal_name_in)
{
    auto signal = wait_job<shared_t<signal_t>>([&] {
        auto lock = graph->get_object_lock();
        return graph->get_signal(signal_name_in);
    });

    signal->wait();
}

} // namespace jackalope
