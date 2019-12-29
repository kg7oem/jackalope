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

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

static source_library_t * source_library = new source_library_t();
static sink_library_t * sink_library = new sink_library_t();

void add_source_constructor(const string_t& class_name_in, source_library_t::constructor_t constructor_in)
{
    source_library->add_constructor(class_name_in, constructor_in);
}

void add_sink_constructor(const string_t& class_name_in, sink_library_t::constructor_t constructor_in)
{
    sink_library->add_constructor(class_name_in, constructor_in);
}

link_t::link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in)
: from(from_in), to(to_in)
{
    assert(from_in != nullptr);
    assert(to_in != nullptr);
}

shared_t<source_t> link_t::get_from()
{
    return from.lock();
}

shared_t<sink_t> link_t::get_to()
{
    return to.lock();
}

channel_t::channel_t(const string_t& name_in, const string_t& type_in)
: name(name_in), type(type_in)
{ }

void channel_t::add_link(shared_t<link_t> link_in)
{
    links.push_back(link_in);
}

void channel_t::init()
{ }

void channel_t::activate()
{ }

shared_t<source_t> source_t::make(const string_t& name_in, const string_t& type_in)
{
    auto source = source_library->make(type_in, name_in, type_in);
    source->init();
    return source;
}

source_t::source_t(const string_t& name_in, const string_t& type_in)
: channel_t(name_in, type_in)
{ }

void source_t::link(shared_t<sink_t> sink_in)
{
    auto link = jackalope::make_shared<link_t>(shared_obj(), sink_in);
    add_link(link);
    sink_in->add_link(link);
}

void source_t::reset()
{ }

shared_t<sink_t> sink_t::make(const string_t& name_in, const string_t& type_in)
{
    auto sink = sink_library->make(type_in, name_in, type_in);
    sink->init();
    return sink;
}

sink_t::sink_t(const string_t& name_in, const string_t& type_in)
: channel_t(name_in, type_in)
{ }

void sink_t::reset()
{ }

shared_t<source_t> channel_obj_t::add_source(const string_t& name_in, const string_t& type_in)
{
    if (sources_by_name.find(name_in) != sources_by_name.end()) {
        throw_runtime_error("Attempt to add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in);
    source->activate();

    sources_by_name[name_in] = source;
    sources.push_back(source);

    return source;
}

shared_t<source_t> channel_obj_t::get_source(const string_t& name_in)
{
    auto found = sources_by_name.find(name_in);

    if (found == sources_by_name.end()) {
        throw_runtime_error("Unknown source name: ", name_in);
    }

    return found->second;
}

shared_t<source_t> channel_obj_t::get_source(const size_t number_in)
{
    if (number_in >= sources.size()) {
        throw_runtime_error("Request for invalid source number: ", number_in);
    }

    return sources[number_in];
}

const pool_vector_t<shared_t<source_t>>& channel_obj_t::get_sources()
{
    return sources;
}

shared_t<sink_t> channel_obj_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    if (sinks_by_name.find(name_in) != sinks_by_name.end()) {
        throw_runtime_error("Attempt to add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in);
    sink->activate();

    sinks_by_name[name_in] = sink;
    sinks.push_back(sink);

    return sink;
}

shared_t<sink_t> channel_obj_t::get_sink(const string_t& name_in)
{
    auto found = sinks_by_name.find(name_in);

    if (found == sinks_by_name.end()) {
        throw_runtime_error("Unknown sink name: ", name_in);
    }

    return found->second;
}

shared_t<sink_t> channel_obj_t::get_sink(const size_t number_in)
{
    if (number_in >= sinks.size()) {
        throw_runtime_error("Request for invalid sink number: ", number_in);
    }

    return sinks[number_in];
}

const pool_vector_t<shared_t<sink_t>>& channel_obj_t::get_sinks()
{
    return sinks;
}

} // namespace jackalope
