// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/object.h>

namespace jackalope {

object_t::object_t(const init_list_t& init_list_in)
: init_args(init_list_in)
{ }

void object_t::init()
{
    auto lock = get_object_lock();
    init__e();
}

void object_t::init__e()
{
    assert_lockable_owner();
}

void object_t::activate()
{
    auto lock = get_object_lock();
    activate__e();
}

void object_t::activate__e()
{
    assert_lockable_owner();
}

void object_t::run()
{
    auto lock = get_object_lock();
    run__e();
}

void object_t::run__e()
{
    assert_lockable_owner();
}

void object_t::stop()
{
    auto lock = get_object_lock();
    stop__e();
}

void object_t::stop__e()
{
    assert_lockable_owner();
}

} // namespace jackalope
