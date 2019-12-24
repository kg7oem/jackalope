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

class foreign_graph_t;
class foreign_object_t;
class object_t;
class graph_t;

shared_t<foreign_graph_t> make_graph(const init_list_t& init_list_in);

class foreign_object_t : public base_t {

protected:
    shared_t<object_t> object = nullptr;

public:
    foreign_object_t(shared_t<object_t> object_in);
    void link(const string_t& source_name_in, shared_t<foreign_object_t> target_object_in, const string_t& target_sink_in);
};

class foreign_graph_t : public base_t {

protected:
    shared_t<graph_t> graph = nullptr;

public:
    foreign_graph_t(shared_t<graph_t> graph_in);
    shared_t<foreign_object_t> add_object(const init_list_t& init_list_in);
    void run();
    void start();
};

} // namespace jackalope
