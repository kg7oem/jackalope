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

    add_signal(JACKALOPE_SIGNAL_OBJECT_STARTED);
    add_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED);

    add_slot(JACKALOPE_SLOT_OBJECT_START, std::bind(&object_t::start, this));
    add_slot(JACKALOPE_SLOT_OBJECT_STOP, std::bind(&object_t::stop, this));
}

void object_t::activate()
{
    assert_lockable_owner();
}

void object_t::start()
{
    assert_lockable_owner();

    get_signal(JACKALOPE_SIGNAL_OBJECT_STARTED)->send();
}

void object_t::stop()
{
    assert_lockable_owner();

    get_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED)->send();
}

shared_t<signal_t> object_t::add_signal(const string_t& name_in)
{
    assert_lockable_owner();

    if (signals.find(name_in) != signals.end()) {
        throw_runtime_error("Duplicate signal name: ", name_in);
    }

    auto signal = jackalope::make_shared<signal_t>(name_in);
    signals.insert({ name_in, signal });

    return signal;
}

shared_t<signal_t> object_t::add_signal(const string_t& name_in, slot_function_t handler_in)
{
    assert_lockable_owner();

    auto signal = add_signal(name_in);
    signal->connect(handler_in);

    return signal;
}

shared_t<signal_t> object_t::get_signal(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = signals.find(name_in);

    if (found == signals.end()) {
        throw_runtime_error("Could not find a signal: ", name_in);
    }

    return found->second;
}

shared_t<slot_t> object_t::add_slot(const string_t& name_in, slot_function_t handler_in)
{
    assert_lockable_owner();

    if (slots.find(name_in) != slots.end()) {
        throw_runtime_error("Duplicate slot name: ", name_in);
    }

    auto shared_this = shared_obj();
    auto new_slot = jackalope::make_shared<slot_t>(name_in, [shared_this, handler_in] {
        auto lock = shared_this->get_object_lock();
        handler_in();
    });
    slots.insert({ name_in, new_slot });

    return new_slot;
}

shared_t<slot_t> object_t::get_slot(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = slots.find(name_in);

    if (found == slots.end()) {
        throw_runtime_error("could not find a slot: ", name_in);
    }

    return found->second;
}

shared_t<source_t> object_t::add_source(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sources.find(name_in) != sources.end()) {
        throw_runtime_error("Attempt to add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in, shared_obj());
    source->activate();
    sources[name_in] = source;

    return source;
}

shared_t<source_t> object_t::get_source(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = sources.find(name_in);

    if (found == sources.end()) {
        throw_runtime_error("Unknown source name: ", name_in);
    }

    return found->second;
}

shared_t<sink_t> object_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sinks.find(name_in) != sinks.end()) {
        throw_runtime_error("Attempt to add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in, shared_obj());
    sink->activate();
    sinks[name_in] = sink;

    return sink;
}

shared_t<sink_t> object_t::get_sink(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = sinks.find(name_in);

    if (found == sinks.end()) {
        throw_runtime_error("Unknown sink name: ", name_in);
    }

    return found->second;
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto source = get_source(source_name_in);
    auto sink = target_in->get_sink(sink_name_in);

    source->link(sink);
}

} // namespace jackalope
