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

#include <jackalope/channel.h>
#include <jackalope/library.h>
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_SIGNAL_OBJECT_STARTED    "object.started"
#define JACKALOPE_SIGNAL_OBJECT_STOPPED    "object.stopped"
#define JACKALOPE_SLOT_OBJECT_START        "object.start"
#define JACKALOPE_SLOT_OBJECT_STOP         "object.stop"

namespace jackalope {

class graph_t;
class object_t;

/*
 *
 * Object Lifecycle
 *
 * construct
 * init - shared this now available
 * start - no more connection/link changes
 * stop
 *
 */

class object_t : public prop_obj_t, public signal_obj_t, public channel_obj_t, public lockable_t, public shared_obj_t<object_t> {

using stop_promise_t = promise_t<void>;

protected:
    bool initialized = false;
    bool started = false;

    object_t(const init_list_t& init_list_in);

public:
    virtual shared_t<signal_t> add_signal(const string_t& name_in) override;
    virtual shared_t<signal_t> add_signal(const string_t& name_in, slot_function_t handler_in);
    virtual shared_t<signal_t> get_signal(const string_t& name_in) override;
    virtual shared_t<slot_t> add_slot(const string_t& name_in, slot_function_t handler_in);
    virtual shared_t<slot_t> get_slot(const string_t& name_in);
    virtual shared_t<source_t> add_source(const string_t& name_in, const string_t& type_in) override;
    virtual shared_t<source_t> get_source(const string_t& name_in) override;
    virtual shared_t<source_t> get_source(const size_t number_in) override;
    virtual const pool_vector_t<shared_t<source_t>>& get_sources() override;
    virtual shared_t<sink_t> add_sink(const string_t& name_in, const string_t& type_in) override;
    virtual shared_t<sink_t> get_sink(const string_t& name_in) override;
    virtual shared_t<sink_t> get_sink(const size_t number_in) override;
    virtual const pool_vector_t<shared_t<sink_t>>& get_sinks() override;

    const init_args_t init_args;
    virtual void init();
    virtual void start();
    virtual void stop();
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in);
};

} // namespace jackalope
