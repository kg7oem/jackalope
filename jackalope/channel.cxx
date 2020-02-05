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

#include <cassert>

#include <jackalope/async.h>
#include <jackalope/channel.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

namespace jackalope {

static source_library_t * source_library = new source_library_t();
static sink_library_t * sink_library = new sink_library_t();

const string_t link_available_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE;

link_available_message_t::link_available_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE, link_in)
{
    assert(link_in != nullptr);
}

const string_t link_ready_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_READY;

link_ready_message_t::link_ready_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_READY, link_in)
{
    assert(link_in != nullptr);
}

const string_t sink_ready_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_SINK_READY;

sink_ready_message_t::sink_ready_message_t(shared_t<sink_t> sink_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_SINK_READY, sink_in)
{
    assert(sink_in != nullptr);
}

const string_t source_available_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_SOURCE_AVAILABLE;

source_available_message_t::source_available_message_t(shared_t<source_t> source_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_SOURCE_AVAILABLE, source_in)
{
    assert(source_in != nullptr);
}

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

string_t link_t::description()
{
    auto source = get_from();
    auto sink = get_to();

    return to_string(source->get_parent()->id, ":", source->name, " -> ", sink->get_parent()->id, ":", sink->name);
}

channel_t::channel_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in)
: parent(parent_in), name(name_in), type(type_in)
{
    assert(parent_in != nullptr);
    assert(name != "");
    assert(type != "");
}

void channel_t::_add_link(shared_t<link_t> link_in)
{
    assert_lockable_owner();

    links.push_back(link_in);
}

void channel_t::_start()
{
    assert_lockable_owner();

    started = true;
}

void channel_t::start()
{
    auto lock = get_object_lock();
    _start();
}

shared_t<source_t> source_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto constructor = source_library->get_constructor(type_in);
    return constructor(name_in, parent_in);
}

source_t::source_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

// thread safe because parent is const
shared_t<object_t> channel_t::get_parent()
{
    return parent.lock();
}

void source_t::_start()
{
    assert_lockable_owner();

    channel_t::_start();
}

void source_t::link(shared_t<sink_t> sink_in)
{
    auto lock = get_object_lock();
    auto new_link = make_link(shared_obj(), sink_in);

    _add_link(new_link);
    sink_in->add_link(new_link);
}

bool source_t::is_available()
{
    auto lock = get_object_lock();

    return _is_available();
}

void source_t::link_available(NDEBUG_UNUSED shared_t<link_t> link_in)
{
    auto lock = get_object_lock();

    auto us = shared_obj();

    assert(link_in->get_from() == us);

    if (_is_available()) {
        get_parent()->send_message<source_available_message_t>(us);
    }
}

void source_t::notify()
{
    auto lock = get_object_lock();
    _notify();
}

void source_t::_notify()
{
    assert_lockable_owner();

    for(auto i : links) {
        i->get_to()->get_parent()->send_message<link_ready_message_t>(i);
    }
}

shared_t<sink_t> sink_t::make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    auto constructor = sink_library->get_constructor(type_in);
    return constructor(name_in, parent_in);
}

sink_t::sink_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in)
: channel_t(name_in, type_in, parent_in)
{ }

void sink_t::add_link(shared_t<link_t> link_in)
{
    auto lock = get_object_lock();

    _add_link(link_in);
}

void sink_t::_start()
{
    assert_lockable_owner();

    channel_t::_start();
}

void sink_t::reset()
{
    auto lock = get_object_lock();
    return _reset();
}

bool sink_t::is_available()
{
    auto lock = get_object_lock();

    return _is_available();
}

bool sink_t::is_ready()
{
    auto lock = get_object_lock();

    return _is_ready();
}

void sink_t::link_ready(NDEBUG_UNUSED shared_t<link_t> link_in)
{
    auto lock = get_object_lock();

    auto us = shared_obj();

    assert(link_in->get_to() == shared_obj());

    if (_is_ready()) {
        get_parent()->send_message<sink_ready_message_t>(us);
    }
}

void sink_t::forward(shared_t<source_t> source_in)
{
    auto lock = get_object_lock();
    _forward(source_in);
}

} //namespace jackalope
