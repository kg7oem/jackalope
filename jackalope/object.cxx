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

static const pool_vector_t<string_t> object_signal_names = {
     JACKALOPE_SIGNAL_OBJECT_DID_ACTIVATE, JACKALOPE_SIGNAL_OBJECT_WILL_ACTTIVATE,
     JACKALOPE_SIGNAL_OBJECT_DID_INIT, JACKALOPE_SIGNAL_OBJECT_WILL_INIT
};

size_t object_t::next_object_id()
{
    static atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);
    return ++current_id;
}

object_t::object_t(const init_args_t& init_args_in)
: init_args(init_args_in)
{ }

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

    add_property(JACKALOPE_PROPERTY_OBJECT_TYPE, property_t::type_t::string, get_type());

    add_message_handler<invoke_slot_message_t>([&] (const string_t& slot_name_in) { this->message_invoke_slot(slot_name_in); });

    for(auto& i : object_signal_names) {
        add_signal(i);
    }
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

shared_t<property_t> object_t::_add_property(const string_t& name_in, const property_t::type_t type_in)
{
    assert_lockable_owner();
    assert_mutex_owner(property_mutex);

    auto property_default = get_property_default(name_in);

    if (property_default.first) {
        return prop_obj_t::_add_property(name_in, type_in, property_default.second);
    }

    return prop_obj_t::_add_property(name_in, type_in);
}

std::pair<bool, string_t> object_t::get_property_default(const string_t& name_in)
{
    assert_lockable_owner();

    if (init_args_has(name_in, init_args)) {
        return { true, init_args_get(name_in, init_args) };
    }

    return { false, "" };
}

// this method has special locking requirements
void object_t::_send_message(shared_t<abstract_message_t> message_in)
{
    lock_t message_lock(message_mutex);

    message_queue.push_back(message_in);

    if (! message_delivering_flag) {
        auto shared_this = shared_obj();

        message_delivering_flag = true;

        async_engine->submit_job([shared_this] {
            shared_this->deliver_messages();
        });
    }
}

bool object_t::should_deliver()
{
    auto lock = get_object_lock();

    if (stopped_flag) {
        return false;
    }

    return true;
}

void object_t::message_invoke_slot(const string_t slot_name_in)
{
    assert_lockable_owner();

    auto slot = get_slot(slot_name_in);
    slot->invoke();
}

} //namespace jackalope
