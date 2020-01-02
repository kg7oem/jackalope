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

#pragma once

#include <jackalope/channel.forward.h>
#include <jackalope/library.h>
#include <jackalope/object.forward.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

using source_library_t = library_t<source_t, const string_t&, shared_t<object_t>>;
using sink_library_t = library_t<sink_t, const string_t&, shared_t<object_t>>;

void add_source_constructor(const string_t& type_name_in, source_library_t::constructor_t constructor_in);
void add_sink_constructor(const string_t& type_name_in, sink_library_t::constructor_t constructor_in);

struct link_t : public base_t, public shared_obj_t<link_t> {

protected:
    const weak_t<source_t> from;
    const weak_t<sink_t> to;

public:

    link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);

    template <class T = source_t>
    shared_t<T> get_from()
    {
        return dynamic_pointer_cast<T>(from.lock());
    }

    template <class T = sink_t>
    shared_t<T> get_to()
    {
        return dynamic_pointer_cast<T>(to.lock());
    }

    virtual bool is_available() = 0;
    virtual bool is_ready() = 0;
};

struct channel_t : public base_t, protected lockable_t {

protected:
    const weak_t<object_t> parent;
    pool_list_t<shared_t<link_t>> links;
    bool started = false;

    virtual void _add_link(shared_t<link_t> link_in);

public:
    const string_t name;
    const string_t type;

    channel_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in);
    virtual ~channel_t() = default;
    shared_t<object_t> get_parent();

    virtual void _start();
    virtual void start();
};

struct source_t : public channel_t, public shared_obj_t<source_t> {

protected:
    bool known_available = false;
    source_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in);

public:
    static shared_t<source_t> make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in);
    virtual ~source_t() = default;
    virtual void _start() override;
    virtual void link(shared_t<sink_t> sink_in);
    virtual shared_t<link_t> make_link(shared_t<source_t> from_in, shared_t<sink_t> to_in) = 0;
    virtual bool is_available();
    virtual bool _is_available() = 0;
    virtual void _check_available();
    virtual void link_available(shared_t<link_t> link_in);
    // virtual void link_unavailable(shared_t<link_t> link_in);
    virtual void _notify_source_available();
    // virtual void _notify_source_unavailable();
};

struct sink_t : public channel_t, public shared_obj_t<sink_t> {

protected:
    bool known_ready = false;

    sink_t(const string_t name_in, const string_t& type_in, shared_t<object_t> parent_in);

public:
    static shared_t<sink_t> make(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in);
    virtual ~sink_t() = default;
    virtual void add_link(shared_t<link_t> link_in);
    virtual void reset();
    virtual void _reset();
    virtual void _start() override;
    virtual bool is_ready();
    virtual bool _is_ready() = 0;
    virtual void _check_ready();
    virtual void link_ready(shared_t<link_t> ready_link_in);
    virtual void _notify_sink_ready();
};

} //namespace jackalope
