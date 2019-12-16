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

#include <jackalope/domain.h>

namespace jackalope {

void domain_t::init()
{
    auto lock = get_object_lock();
    init__e();
}

void domain_t::init__e()
{
    assert_lockable_owner();
}

void domain_t::run()
{
    auto lock = get_object_lock();
    run__e();
}

void domain_t::run__e()
{
    assert_lockable_owner();
}

shared_t<driver_t> domain_t::add_driver(const init_list_t&)
{
    return make_shared<driver_t>();
}

shared_t<node_t> domain_t::add_node(const init_list_t&)
{
    return make_shared<node_t>();
}

} // namespace jackalope
