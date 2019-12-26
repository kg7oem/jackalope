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

#include <jackalope/property.h>
#include <jackalope/types.h>

namespace jackalope {

struct foreign_graph_t;
struct foreign_node_t;
class node_t;
class graph_t;

shared_t<foreign_graph_t> make_graph(const init_list_t& init_list_in);

struct foreign_node_t : public base_t {

    shared_t<node_t> node = nullptr;

    foreign_node_t(shared_t<node_t> node_in);
    void connect(const string_t& signal_name_in, shared_t<foreign_node_t> target_node_in, const string_t& target_slot_in);
    void connect(const string_t& signal_name_in, shared_t<foreign_graph_t> target_graph_in, const string_t& target_slot_in);
    void link(const string_t& source_name_in, shared_t<foreign_node_t> target_node_in, const string_t& target_sink_in);
};

struct foreign_graph_t : public base_t {

    shared_t<graph_t> graph = nullptr;

    foreign_graph_t(shared_t<graph_t> graph_in);
    shared_t<foreign_node_t> add_node(const init_list_t& init_list_in);
    void start();
    void stop();
    virtual void wait_signal(const string_t& signal_name_in);
};

} // namespace jackalope
