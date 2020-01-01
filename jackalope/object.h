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

#include <jackalope/channel.h>
#include <jackalope/foreign.forward.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class object_t : public shared_obj_t<object_t>, protected lockable_t, public base_t {

    friend foreign::node_t;

protected:
    const init_args_t init_args;
    bool running_flag = false;
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    bool sources_known_available = false;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;
    bool sinks_known_ready = false;

public:
    template <class T = object_t, typename... Args>
    static shared_t<T> make(Args... args)
    {
        auto new_object = jackalope::make_shared<T>(args...);
        auto lock = new_object->get_object_lock();
        new_object->init();
        return new_object;
    }

    object_t(const init_list_t init_list_in);

    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const size_t source_num_in);
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& sink_name_in);
    virtual shared_t<sink_t> get_sink(const size_t sink_num_in);
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_object_in, const string_t& target_sink_name_in);
    virtual void init();
    virtual void start();
    virtual void stop();
    virtual void source_available(shared_t<source_t>source_in);
    virtual void slot_source_available(shared_t<source_t> source_in);
    virtual void source_unavailable(shared_t<source_t> source_in);
    virtual void slot_source_unavailable(shared_t<source_t> source_in);
    virtual void check_sources_available();
    virtual void all_sources_available();
    virtual void sink_ready(shared_t<sink_t> ready_sink_in);
    virtual void slot_sink_ready(shared_t<sink_t> ready_sink_in);
    virtual void check_sinks_ready();
    virtual void all_sinks_ready();
};

} //namespace jackalope
