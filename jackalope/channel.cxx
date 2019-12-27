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
    assert_object_owner(get_parent());

    links.push_back(link_in);
}

void channel_t::init()
{
    assert_object_owner(get_parent());

    add_signal(JACKALOPE_SIGNAL_CHANNEL_AVAILABLE);
    add_signal(JACKALOPE_SIGNAL_CHANNEL_READY);
}

void channel_t::activate()
{
    assert_object_owner(get_parent());
}

shared_t<source_t> source_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto source = source_library->make(type_in, name_in, type_in, parent_in);
    source->init();
    return source;
}

source_t::source_t(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

void source_t::link(shared_t<sink_t> sink_in)
{
    assert_object_owner(get_parent());
    assert_object_owner(sink_in->get_parent());

    auto link = jackalope::make_shared<link_t>(shared_obj(), sink_in);
    add_link(link);
    sink_in->add_link(link);
}

void source_t::reset()
{
    assert_object_owner(get_parent());

    get_signal(JACKALOPE_SIGNAL_CHANNEL_AVAILABLE)->send();
}

shared_t<sink_t> sink_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto sink = sink_library->make(type_in, name_in, type_in, parent_in);
    sink->init();
    return sink;
}

sink_t::sink_t(const string_t& name_in, const string_t& type_in,  shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

void sink_t::reset()
{
    assert_object_owner(get_parent());

    get_signal(JACKALOPE_SIGNAL_CHANNEL_READY)->send();
}

void sink_t::source_ready(shared_t<source_t> source_in)
{
    assert_object_owner(get_parent());
}

} // namespace jackalope
