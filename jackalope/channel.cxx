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
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

static pool_map_t<string_t, source_constructor_t> source_constructors;
static pool_map_t<string_t, sink_constructor_t> sink_constructors;

void add_source_constructor(const string_t& class_name_in, source_constructor_t constructor_in)
{
    if (source_constructors.find(class_name_in) != source_constructors.end()) {
        throw_runtime_error("attempt to add duplicate source constuctor: ", class_name_in);
    }

    source_constructors[class_name_in] = constructor_in;
}

void add_sink_constructor(const string_t& class_name_in, sink_constructor_t constructor_in)
{
    if (sink_constructors.find(class_name_in) != sink_constructors.end()) {
        throw_runtime_error("attempt to add duplicate sink constuctor: ", class_name_in);
    }

    sink_constructors[class_name_in] = constructor_in;
}

source_constructor_t get_source_constructor(const string_t& class_name_in)
{
    auto found = source_constructors.find(class_name_in);

    if (found == source_constructors.end()) {
        throw_runtime_error("could not find source constructor: ", class_name_in);
    }

    return found->second;
}

sink_constructor_t get_sink_constructor(const string_t& class_name_in)
{
    auto found = sink_constructors.find(class_name_in);

    if (found == sink_constructors.end()) {
        throw_runtime_error("could not find sink constructor: ", class_name_in);
    }

    return found->second;
}

link_t::link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in)
: from(from_in), to(to_in)
{ }

channel_t::channel_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
: parent(parent_in), name(name_in), type(type_in)
{ }

shared_t<object_t> channel_t::get_parent()
{
    assert(! parent.expired());

    return parent.lock();
}

void channel_t::add_link(shared_t<link_t> link_in)
{
    links.push_back(link_in);
}

void channel_t::init()
{ }

void channel_t::activate()
{ }

shared_t<source_t> source_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto constructor = get_source_constructor(type_in);
    auto source = constructor(name_in, type_in, parent_in);
    source->init();
    return source;
}

source_t::source_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

void source_t::link(shared_t<sink_t> sink_in)
{
    auto link = jackalope::make_shared<link_t>(shared_obj(), sink_in);
    add_link(link);
    sink_in->add_link(link);
}

shared_t<sink_t> sink_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto constructor = get_sink_constructor(type_in);
    auto sink = constructor(name_in, type_in, parent_in);
    sink->init();
    return sink;
}

sink_t::sink_t(const string_t& name_in, const string_t& type_in,  shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

} // namespace jackalope
