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

size_t object_t::next_object_id()
{
    atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);
    return ++current_id;
}

object_t::object_t(const init_args_t& init_args_in)
: init_args(init_args_in)
{ }

shared_t<property_t> object_t::add_property(const string_t& name_in, const property_t::type_t type_in)
{
    assert_lockable_owner();

    auto property_default = get_property_default(name_in);
    auto property = property_t::make(type_in);

    if (property_default.first) {
        property->set(property_default.second);
    }

    return property;
}

std::pair<bool, string_t> object_t::get_property_default(const string_t& name_in)
{
    assert_lockable_owner();

    if (init_args_has(name_in, init_args)) {
        return { true, init_args_get(name_in, init_args) };
    }

    return { false, "" };
}

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
    return to_string("object #", id, " ", get_type());
}

} //namespace jackalope
