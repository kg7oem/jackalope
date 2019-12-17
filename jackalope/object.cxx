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

shared_t<source_t> object_t::add_source(const string_t& name_in, const string_t& type_in)
{
    if (sources.find(name_in) != sources.end()) {
        throw_runtime_error("Attempt to add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in, shared_obj());
    source->activate();
    sources[name_in] = source;

    return source;
}

shared_t<sink_t> object_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    if (sinks.find(name_in) != sinks.end()) {
        throw_runtime_error("Attempt to add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in, shared_obj());
    sink->activate();
    sinks[name_in] = sink;

    return sink;
}

} // namespace jackalope
