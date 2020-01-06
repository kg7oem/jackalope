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

#include <jackalope/object.forward.h>
#include <jackalope/node.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_TYPE_GRAPH "jackalope::graph"

namespace jackalope {

class graph_t : public object_t {

public:
    using prop_args_t = pool_vector_t<std::pair<const string_t, property_t::type_t>>;

protected:
    pool_list_t<shared_t<node_t>> nodes;

public:
    static shared_t<graph_t> make(const init_args_t& init_args_in = {});
    static shared_t<graph_t> make(const prop_args_t& prop_args_in);
    graph_t(const init_args_t& init_args_in);
    graph_t(const prop_args_t& prop_args_in);
    virtual shared_t<property_t> add_property(const string_t& name_in, property_t::type_t type_in) override;
    virtual shared_t<property_t> add_property(const string_t& name_in, property_t::type_t type_in, const init_args_t& init_args_in) override;
    shared_t<node_t> add_node(const init_args_t& init_args_in);
    virtual void init() override;
    virtual void start() override;
    virtual void stop() override;
};

} // namespace jackalope
