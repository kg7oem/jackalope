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

#include <jackalope/foreign.forward.h>

#ifdef __cplusplus

#include <jackalope/graph.h>
#include <jackalope/network.h>
#include <jackalope/property.h>
#include <jackalope/types.h>

extern "C" {
#endif // __cplusplus

struct dbus_objectAdaptee;

void jackalope_init();

void jackalope_object_delete(struct jackalope_object_t * object_in);
void jackalope_object_connect(struct jackalope_object_t * object_in, const char * signal_in, struct jackalope_object_t * target_object_in, const char * slot_in);
void jackalope_object_start(struct jackalope_object_t * object_in);
void jackalope_object_stop(struct jackalope_object_t * object_in);

struct jackalope_object_t * jackalope_graph_make(const char * init_args_in[]);
struct jackalope_object_t * jackalope_graph_add_node(struct jackalope_object_t * graph_in, const char * init_args_in[]);
void jackalope_graph_run(struct jackalope_object_t * graph_in);

struct jackalope_object_t * jackalope_node_make(const char ** init_args_in);
struct jackalope_source_t * jackalope_node_add_source(struct jackalope_object_t * object_in, const char * type_in, const char * name_in);
unsigned int jackalope_node_get_num_sources(struct jackalope_object_t * object_in);
struct jackalope_sink_t * jackalope_node_add_sink(struct jackalope_object_t * object_in, const char * type_in, const char * name_in);
unsigned int jackalope_node_get_num_sinks(struct jackalope_object_t * object_in);
void jackalope_node_link(struct jackalope_object_t * object_in, const char * source_in, struct jackalope_object_t * target_object_in, const char * sink_in);

#ifdef __cplusplus
}

template <typename T>
struct jackalope_wrapper_t {

    const jackalope::shared_t<T> wrapped;

    jackalope_wrapper_t(jackalope::shared_t<T> wrapped_in)
    : wrapped(wrapped_in)
    {
        assert(wrapped_in != nullptr);
    }
};

struct jackalope_source_t : public jackalope_wrapper_t<jackalope::source_t> {
    jackalope_source_t(jackalope::shared_t<jackalope::source_t> wrapped_in);
};

struct jackalope_sink_t : public jackalope_wrapper_t<jackalope::sink_t> {
    jackalope_sink_t(jackalope::shared_t<jackalope::sink_t> wrapped_in);
};

struct jackalope_object_t : public jackalope_wrapper_t<jackalope::object_t> {
    jackalope_object_t(jackalope::shared_t<jackalope::object_t> wrapped_in);
    virtual ~jackalope_object_t() = default;
    jackalope::string_t peek(const jackalope::string_t& property_name_in);
    void poke(const jackalope::string_t& property_name_in, const jackalope::string_t& value_in);
    virtual void connect(const jackalope::string_t& signal_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& slot_name_in);
    virtual void start();
    virtual void stop();

    template <typename T>
    T wait_job(jackalope::async_job_t<T> job_in)
    {
        jackalope::promise_t<T> promise;

        wrapped->async_engine->submit_job([&] {
            auto result = job_in();
            promise.set_value(result);
        });

        return promise.get_future().get();
    }

    inline void wait_job(jackalope::async_job_t<void> job_in)
    {
        jackalope::promise_t<void> promise;

        wrapped->async_engine->submit_job([&] {
            job_in();
            promise.set_value();
        });

        promise.get_future().get();
    }
};

struct jackalope_graph_t : public jackalope_object_t {

    static jackalope_graph_t make(const jackalope::init_args_t& init_args_in);
    static jackalope_graph_t make(const jackalope::graph_t::prop_args_t& prop_args_in);
    jackalope_graph_t(jackalope::shared_t<jackalope::graph_t> wrapped_in);
    virtual void add_property(const jackalope::string_t& name_in, jackalope::property_t::type_t type_in);
    virtual void add_property(const jackalope::string_t& name_in, jackalope::property_t::type_t type_in, const jackalope::init_args_t& init_args_in);
    virtual jackalope_node_t make_node(const jackalope::init_args_t& init_args_in);
    virtual jackalope_node_t add_node(const jackalope::init_args_t& init_args_in);
    virtual jackalope_network_t make_network(const jackalope::init_args_t& init_args_in);
    virtual jackalope_network_t add_network(const jackalope::init_args_t& init_args_in);
    virtual void run();
};

struct jackalope_node_t : public jackalope_object_t {
    virtual ~jackalope_node_t() = default;
    static jackalope_node_t make(const jackalope::init_args_t& init_args_in);
    jackalope_node_t(jackalope::shared_t<jackalope::node_t> wrapped_in);
    virtual jackalope_source_t add_source(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    virtual jackalope::size_t get_num_sources();
    virtual jackalope_sink_t add_sink(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    virtual jackalope::size_t get_num_sinks();
    virtual void activate();
    virtual void link(const jackalope::string_t& source_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& target_sink_name_in);
    virtual void forward(const jackalope::string_t& source_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& target_sink_name_in);
};

struct jackalope_network_t : public jackalope_node_t {
    static jackalope_network_t make(const jackalope::init_args_t& init_args_in);
    jackalope_network_t(jackalope::shared_t<jackalope::network_t> wrapped_in);
};

#endif // __cplusplus
