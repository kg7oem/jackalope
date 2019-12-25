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

graph_t::graph_t(const init_list_t& init_args_in)
: object_t(init_args_in)
{ }

shared_t<object_t> graph_t::add_object(const init_list_t& init_args_in)
{
    assert_lockable_owner();

    auto new_object = object_t::make(init_args_in);
    auto new_object_lock = new_object->get_object_lock();

    new_object->activate();

    objects.push_back(new_object);

    return new_object;
}

} // namespace jackalope
