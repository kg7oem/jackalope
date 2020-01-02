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

    _check_available();
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

void source_t::link_available(shared_t<link_t>)
{
    auto lock = get_object_lock();

    if (! started) {
        return;
    }

    _check_available();
}

void source_t::link_unavailable(shared_t<link_t>)
{
    auto lock = get_object_lock();

    if (! started) {
        return;
    }

    _check_available();
}

void source_t::_check_available()
{
    assert_lockable_owner();

    if (_is_available() && ! known_available) {
        known_available = true;
        _notify_source_available();
    }

    if (! _is_available() && known_available) {
        known_available = false;
        _notify_source_unavailable();
    }
}

void source_t::_notify_source_available()
{
    assert_lockable_owner();

    auto parent = get_parent();
    auto shared_this = shared_obj();

    assert(known_available == true);

    submit_job([parent, shared_this] { parent->slot_source_available(shared_this); });
}

void source_t::_notify_source_unavailable()
{
    assert_lockable_owner();

    auto parent = get_parent();
    auto shared_this = shared_obj();

    assert(known_available == false);

    submit_job([parent, shared_this] { parent->slot_source_unavailable(shared_this); });
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

void sink_t::reset()
{
    auto lock = get_object_lock();

    _reset();
}

void sink_t::_reset()
{
    assert_lockable_owner();

    _check_ready();
}

void sink_t::_start()
{
    assert_lockable_owner();

    channel_t::_start();

    _reset();
}

bool sink_t::is_ready()
{
    auto lock = get_object_lock();

    return _is_ready();
}

void sink_t::link_ready(shared_t<link_t> link_in)
{
    auto lock = get_object_lock();

    log_info("link ready for sink '", name, "': ", link_in->get_from()->name);

    _check_ready();
}

void sink_t::_check_ready()
{
    assert_lockable_owner();

    if (_is_ready() && ! known_ready) {
        known_ready = true;
        _notify_sink_ready();
    }
}

void sink_t::_notify_sink_ready()
{
    assert_lockable_owner();

    auto parent = get_parent();
    auto shared_this = shared_obj();

    assert(known_ready == true);

    submit_job([parent, shared_this] { parent->slot_sink_ready(shared_this); });
}

} //namespace jackalope
