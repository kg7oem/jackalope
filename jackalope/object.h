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
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_OBJECT_PROPERTY_NAME     "object.name"
#define JACKALOPE_OBJECT_PROPERTY_CLASS    "object.class"
#define JACKALOPE_OBJECT_SIGNAL_STOPPED    "object.stopped"

namespace jackalope {

class graph_t;
class object_t;

using object_library_t = library_t<object_t, const init_list_t&>;

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in);

class object_t : public prop_obj_t, public lockable_t, public shared_obj_t<object_t> {

using stop_promise_t = promise_t<void>;

protected:
    const init_list_t init_args;
    weak_t<graph_t> graph;
    pool_map_t<string_t, shared_t<signal_t>> signals;
    pool_map_t<string_t, shared_t<slot_t>> slots;
    pool_map_t<string_t, shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<sink_t>> sinks;
    bool stop_flag = true;
    std::mutex stop_mutex;
    condition_t stop_condition;

    virtual void object_stop_handler(shared_t<signal_t> signal_in);

public:
    static shared_t<object_t> make(const init_list_t& init_list_in);
    object_t(const init_list_t& init_list_in);
    void set_graph(shared_t<graph_t>);
    virtual shared_t<graph_t> get_graph();
    virtual shared_t<signal_t> add_signal(const string_t& name_in);
    virtual shared_t<signal_t> get_signal(const string_t& name_in);
    virtual shared_t<slot_t> add_slot(const string_t& name_in, slot_handler_t handler_in);
    virtual shared_t<slot_t> get_slot(const string_t& name_in);
    virtual shared_t<source_t> add_source(const string_t& name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& name_in);
    virtual shared_t<sink_t> add_sink(const string_t& name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& name_in);
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void stop();
    virtual void wait_stop();
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in);
};

} // namespace jackalope
