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
#include <jackalope/object.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

struct link_t : public base_t, public shared_obj_t<link_t> {

protected:
    const weak_t<source_t> from;
    const weak_t<sink_t> to;

public:
    atomic_t<bool> is_available = ATOMIC_VAR_INIT(false);
    atomic_t<bool> is_ready = ATOMIC_VAR_INIT(false);

    link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);
    shared_t<source_t> get_from();
    shared_t<sink_t> get_to();
};

struct channel_t : public base_t, protected lockable_t {

protected:
    const weak_t<object_t> parent;
    pool_list_t<shared_t<link_t>> links;

public:
    const string_t name;

    channel_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual ~channel_t() = default;
    shared_t<object_t> get_parent();
};

struct source_t : public channel_t, public shared_obj_t<source_t> {

protected:
    bool known_available = false;

public:
    source_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual ~source_t() = default;
    bool is_available();
    void link_available(shared_t<link_t> available_link_in);
    void notify_source_available();
};

struct sink_t : public channel_t, public shared_obj_t<sink_t> {
    sink_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual ~sink_t() = default;
    bool is_ready();
};

} //namespace jackalope
