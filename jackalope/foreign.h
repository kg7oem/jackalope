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

struct jackalope_graph_t * jackalope_graph_make(const char ** init_args_in);
void jackalope_graph_free(struct jackalope_graph_t * graph_in);
struct jackalope_node_t * jackalope_graph_add_node(const char ** init_args_in);
void jackalope_graph_start(struct jackalope_graph_t * graph_in);
void jackalope_graph_run(struct jackalope_graph_t * graph_in);

struct jackalope_node_t * jackalope_node_make(const char ** init_args_in);
void jackalope_node_free(struct jackalope_node_t * node_in);
struct jackalope_source_t * jackalope_node_add_source(const char * type_in, const char * name_in);
struct jackalope_sink_t * jackalope_node_add_sink(const char * type_in, const char * name_in);
void jackalope_node_connect(const char * signal_in, jackalope_graph_t * target_object_in, const char * slot_in);
void jackalope_node_link(const char * source_in, jackalope_node_t * target_object_in, const char * sink_in);

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

struct jackalope_graph_t : public jackalope_wrapper_t<jackalope::graph_t> {

    static jackalope_graph_t make(const jackalope::init_args_t& init_args_in);
    jackalope_graph_t(jackalope::shared_t<jackalope::graph_t> wrapped_in);
    jackalope_node_t add_node(const jackalope::init_list_t& init_args_in);
    void start();
    void run();
};

struct jackalope_source_t : public jackalope_wrapper_t<jackalope::source_t> {
    jackalope_source_t(jackalope::shared_t<jackalope::source_t> wrapped_in);
};

struct jackalope_sink_t : public jackalope_wrapper_t<jackalope::sink_t> {
    jackalope_sink_t(jackalope::shared_t<jackalope::sink_t> wrapped_in);
};

struct jackalope_node_t : public jackalope_wrapper_t<jackalope::node_t> {
    static jackalope_node_t make(const jackalope::init_args_t& init_args_in);
    jackalope_node_t(jackalope::shared_t<jackalope::node_t> wrapped_in);
    jackalope_source_t add_source(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    jackalope_sink_t add_sink(const jackalope::string_t& name_in, const jackalope::string_t& type_in);
    void connect(const jackalope::string_t& signal_name_in, jackalope_graph_t target_in, const jackalope::string_t& slot_name_in);
    void link(const jackalope::string_t& source_name_in, jackalope_node_t target_object_in, const jackalope::string_t& target_sink_name_in);
    void activate();
    void start();
};

#endif // __cplusplus
