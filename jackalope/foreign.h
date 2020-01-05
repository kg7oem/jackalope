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
#include <jackalope/node.h>
#include <jackalope/types.h>

extern "C" {
#endif // __cplusplus

void jackalope_init();
void jackalope_shutdown();

void jackalope_object_delete(struct jackalope_object_t * object_in);
struct jackalope_source_t * jackalope_object_add_source(struct jackalope_object_t * object_in, const char * type_in, const char * name_in);
struct jackalope_sink_t * jackalope_object_add_sink(struct jackalope_object_t * object_in, const char * type_in, const char * name_in);
void jackalope_object_connect(struct jackalope_object_t * object_in, const char * signal_in, struct jackalope_object_t * target_object_in, const char * slot_in);
void jackalope_object_link(struct jackalope_object_t * object_in, const char * source_in, struct jackalope_object_t * target_object_in, const char * sink_in);
void jackalope_object_start(struct jackalope_object_t * object_in);
void jackalope_object_run(struct jackalope_object_t * object_in);
void jackalope_object_stop(struct jackalope_object_t * object_in);

struct jackalope_object_t * jackalope_graph_make(const char * init_args_in[]);
struct jackalope_object_t * jackalope_graph_add_node(struct jackalope_object_t * graph_in, const char * init_args_in[]);
void jackalope_graph_run(struct jackalope_object_t * graph_in);

struct jackalope_object_t * jackalope_node_make(const char ** init_args_in);
void jackalope_node_run(struct jackalope_object_t * node_in);

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
    virtual jackalope_source_t add_source(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    virtual jackalope_sink_t add_sink(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    virtual void connect(const jackalope::string_t& signal_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& slot_name_in);
    virtual void link(const jackalope::string_t& source_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& target_sink_name_in);
    virtual void activate();
    virtual void start();
    virtual void stop();
};

struct jackalope_graph_t : public jackalope_object_t {

    static jackalope_graph_t make(const jackalope::init_args_t& init_args_in);
    jackalope_graph_t(jackalope::shared_t<jackalope::graph_t> wrapped_in);
    virtual jackalope_node_t add_node(const jackalope::init_args_t& init_args_in);
    virtual void run();
};

struct jackalope_node_t : public jackalope_object_t {
    virtual ~jackalope_node_t() = default;
    static jackalope_node_t make(const jackalope::init_args_t& init_args_in);
    jackalope_node_t(jackalope::shared_t<jackalope::node_t> wrapped_in);
    virtual void run();
};

#endif // __cplusplus
