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

#include <jackalope/foreign.h>
#include <jackalope/graph.h>
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

void foreign_object_t::connect(const string_t& , shared_t<foreign_graph_t> , const string_t& )
{ }

void foreign_object_t::link(const string_t& , shared_t<foreign_object_t> , const string_t&)
{ }

foreign_graph_t::foreign_graph_t(shared_t<graph_t> graph_in)
: graph(graph_in)
{
    assert(graph != nullptr);
}

void foreign_graph_t::run()
{
    while(true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}

shared_t<foreign_object_t> foreign_graph_t::add_object(const init_list_t& init_list_in)
{
    auto object = jackalope::make_shared<object_t>(init_list_in);
    return jackalope::make_shared<foreign_object_t>(object);
}

} // namespace jackalope
