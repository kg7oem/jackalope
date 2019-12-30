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
#include <jackalope/object.h>

namespace jackalope {

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

channel_t::channel_t(const string_t name_in, shared_t<object_t> parent_in)
: parent(parent_in), name(name_in)
{
    assert(parent_in != nullptr);
}

// thread safe because parent is const
shared_t<object_t> channel_t::get_parent()
{
    return parent.lock();
}

void source_t::start()
{
    auto lock = get_object_lock();

    _check_available();
}

bool source_t::is_available()
{
    auto lock = get_object_lock();

    return _is_available();
}

// a source is available if none
// of the links are not available
bool source_t::_is_available()
{
    assert_lockable_owner();

    for(auto& i : links) {
        if (! i->is_available) {
            return false;
        }
    }

    return true;
}

void source_t::link_available(shared_t<link_t>)
{
    auto lock = get_object_lock();

    _check_available();
}

void source_t::_check_available()
{
    assert_lockable_owner();

    if (_is_available() && ! known_available) {
        known_available = true;
        _notify_source_available();
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

source_t::source_t(const string_t name_in, shared_t<object_t> parent_in)
: channel_t(name_in, parent_in)
{ }

sink_t::sink_t(const string_t name_in, shared_t<object_t> parent_in)
: channel_t(name_in, parent_in)
{ }

} //namespace jackalope
