// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/object.h>

namespace jackalope {

object_t::object_t(const init_list_t& init_list_in)
: init_args(init_args_from_list(init_list_in))
{ }

void object_t::init()
{
    assert_lockable_owner();

    if (initialized) {
        throw_runtime_error("can not initialize an object that is already initialized");
    }

    add_signal(JACKALOPE_SIGNAL_OBJECT_STARTED);
    add_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED);

    add_slot(JACKALOPE_SLOT_OBJECT_START, std::bind(&object_t::start, this));
    add_slot(JACKALOPE_SLOT_OBJECT_STOP, std::bind(&object_t::stop, this));

    initialized = true;
}

void object_t::start()
{
    assert_lockable_owner();

    if (! initialized) {
        throw_runtime_error("can not start an object that has not been initialized");
    }

    if (started) {
        throw_runtime_error("can not start an object that has already been started");
    }

    started = true;

    get_signal(JACKALOPE_SIGNAL_OBJECT_STARTED)->send();
}

void object_t::stop()
{
    assert_lockable_owner();

    if (! started) {
        throw_runtime_error("can not stop an object that has not been started");
    }

    started = false;

    get_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED)->send();
}

shared_t<signal_t> object_t::add_signal(const string_t& name_in)
{
    assert_lockable_owner();

    return signal_obj_t::add_signal(name_in);
}

shared_t<signal_t> object_t::add_signal(const string_t& name_in, slot_function_t handler_in)
{
    assert_lockable_owner();

    auto new_signal = signal_obj_t::add_signal(name_in);

    new_signal->subscribe([this, handler_in] {
        auto lock = get_object_lock();
        handler_in();
    });

    return new_signal;
}

shared_t<signal_t> object_t::get_signal(const string_t& name_in)
{
    assert_lockable_owner();

    return signal_obj_t::get_signal(name_in);
}

shared_t<slot_t> object_t::add_slot(const string_t& name_in, slot_function_t handler_in)
{
    assert_lockable_owner();

    return signal_obj_t::add_slot(name_in, [this, handler_in] {
        auto lock = get_object_lock();
        handler_in();
    });
}

shared_t<slot_t> object_t::get_slot(const string_t& name_in)
{
    assert_lockable_owner();

    return signal_obj_t::get_slot(name_in);
}

shared_t<source_t> object_t::add_source(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    return channel_obj_t::add_source(name_in, type_in);
}

shared_t<source_t> object_t::get_source(const string_t& name_in)
{
    assert_lockable_owner();

    return channel_obj_t::get_source(name_in);
}

shared_t<source_t> object_t::get_source(const size_t number_in)
{
    assert_lockable_owner();

    return channel_obj_t::get_source(number_in);
}

const pool_vector_t<shared_t<source_t>>& object_t::get_sources()
{
    assert_lockable_owner();

    return channel_obj_t::get_sources();
}

shared_t<sink_t> object_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    return channel_obj_t::add_sink(name_in, type_in);
}

shared_t<sink_t> object_t::get_sink(const string_t& name_in)
{
    assert_lockable_owner();

    return channel_obj_t::get_sink(name_in);
}

shared_t<sink_t> object_t::get_sink(const size_t number_in)
{
    assert_lockable_owner();

    return channel_obj_t::get_sink(number_in);
}

const pool_vector_t<shared_t<sink_t>>& object_t::get_sinks()
{
    assert_lockable_owner();

    return channel_obj_t::get_sinks();
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto source = get_source(source_name_in);
    auto sink = target_in->get_sink(sink_name_in);

    source->link(sink);
}

} // namespace jackalope
