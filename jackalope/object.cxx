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
    assert_lockable_owner();
}

void object_t::activate()
{
    assert_lockable_owner();
}

void object_t::run()
{
    assert_lockable_owner();
}

void object_t::stop()
{
    assert_lockable_owner();
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
