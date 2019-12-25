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
#include <jackalope/foreign.h>
#include <jackalope/graph.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

namespace jackalope {

shared_t<foreign_graph_t> make_graph(const init_list_t& init_list_in)
{
    auto graph = jackalope::make_shared<graph_t>(init_list_in);
    return jackalope::make_shared<foreign_graph_t>(graph);
}

foreign_object_t::foreign_object_t(shared_t<object_t> object_in)
: object(object_in)
{
    assert(object != nullptr);
}

void foreign_object_t::connect(const string_t& , shared_t<foreign_object_t> , const string_t& )
{ }

void foreign_object_t::connect(const string_t& signal_name_in, shared_t<foreign_graph_t> target_graph_in, const string_t& target_slot_in)
{
    wait_job<void>([&] {
        auto our_lock = object->get_object_lock();
        auto target_lock = target_graph_in->graph->get_object_lock();

        auto signal = object->get_signal(signal_name_in);
        auto slot = target_graph_in->graph->get_slot(target_slot_in);

        signal->connect(slot);
    });
}

void foreign_object_t::link(const string_t& , shared_t<foreign_object_t> , const string_t&)
{ }

foreign_graph_t::foreign_graph_t(shared_t<graph_t> graph_in)
: graph(graph_in)
{
    assert(graph != nullptr);
}

void foreign_graph_t::run()
{
    wait_job<void>([this] {
        auto lock = graph->get_object_lock();
        log_info("job inside run() wrapper ran!");
        // graph->run();
    });
}

shared_t<foreign_object_t> foreign_graph_t::add_object(const init_list_t& init_list_in)
{
    shared_t<object_t> new_object = wait_job<shared_t<object_t>>([&] {
        auto lock = graph->get_object_lock();
        return graph->add_object(init_list_in);
    });

    return jackalope::make_shared<foreign_object_t>(new_object);
}

} // namespace jackalope
