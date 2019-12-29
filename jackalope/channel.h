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
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

#define JACKALOPE_SIGNAL_CHANNEL_AVAILABLE     "channel.available"
#define JACKALOPE_SIGNAL_CHANNEL_READY         "channel.ready"

namespace jackalope {

using source_library_t = library_t<source_t, const string_t&, const string_t&>;
using sink_library_t = library_t<sink_t, const string_t&, const string_t&>;

void add_source_constructor(const string_t& class_name_in, source_library_t::constructor_t constructor_in);
void add_sink_constructor(const string_t& class_name_in, sink_library_t::constructor_t constructor_in);

class link_t : base_t, public shared_obj_t<link_t> {

protected:
    const weak_t<source_t> from;
    const weak_t<sink_t> to;

public:
    link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);
    shared_t<source_t> get_from();
    shared_t<sink_t> get_to();
};

class channel_t : base_t, public signal_obj_t {

protected:
    pool_list_t<shared_t<link_t>> links;

    channel_t(const string_t& name_in, const string_t& type_in);

public:
    const string_t name;
    const string_t type;

    virtual void add_link(shared_t<link_t> link_in);
    virtual void init();
    virtual void activate();
};

class source_t : public channel_t, public shared_obj_t<source_t> {

public:
    static shared_t<source_t> make(const string_t& name_in, const string_t& type_in);
    source_t(const string_t& name_in, const string_t& type_in);
    virtual void link(shared_t<sink_t> sink_in);
    virtual void reset();
};

class sink_t : public channel_t, public shared_obj_t<sink_t> {

public:
    static shared_t<sink_t> make(const string_t& name_in, const string_t& type_in);
    sink_t(const string_t& name_in, const string_t& type_in);
    virtual void reset();
};

class channel_obj_t {

protected:
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;

public:
    virtual shared_t<source_t> add_source(const string_t& name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& name_in);
    virtual shared_t<source_t> get_source(const size_t number_in);
    virtual const pool_vector_t<shared_t<source_t>>& get_sources();
    virtual shared_t<sink_t> add_sink(const string_t& name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& name_in);
    virtual shared_t<sink_t> get_sink(const size_t number_in);
    virtual const pool_vector_t<shared_t<sink_t>>& get_sinks();
};

} // namespace jackalope
