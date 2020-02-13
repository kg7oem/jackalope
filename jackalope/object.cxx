// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

namespace jackalope {

void object_t::init()
{
    assert_lockable_owner();

    if (initialized_flag) {
        throw_runtime_error("Can't call init() on a node that has already been initialized: ", description());
    }

    object_log_trace("initializing object");

    will_init();
    initialized_flag = true;
    did_init();

    object_log_trace("Done initializing object");
}

void object_t::will_init()
{
    assert_lockable_owner();
    assert(initialized_flag == false);
}

void object_t::did_init()
{
    assert_lockable_owner();
    assert(initialized_flag);
}

void object_t::activate()
{
    assert_lockable_owner();

    if (! initialized_flag) {
        throw_runtime_error("Can't call activate() on a node that has not been initialized: ", description());
    }

    if (activated_flag) {
        throw_runtime_error("Can't call activate() on a node that is already activated");
    }

    log_trace("activating object");

    will_activate();
    activated_flag = true;
    did_activate();

    log_trace("done activating object");
}

void object_t::will_activate()
{
    assert_lockable_owner();
    assert(activated_flag == false);
}

void object_t::did_activate()
{
    assert_lockable_owner();
    assert(activated_flag);
}

// description() must never require a lock
// even from a subclass
string_t object_t::description()
{
    return to_string("object #", id);
}

} //namespace jackalope
