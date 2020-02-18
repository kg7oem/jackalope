// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#pragma once

#include <jackalope/forward.h>
#include <jackalope/object.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

#define JACKALOPE_PROPERTY_NODE_TYPE "node.type"

namespace jackalope {

class node_t : public object_t {

protected:
    weak_t<project_t> weak_project;

    node_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual void did_stop() override;
    virtual std::pair<bool, string_t> get_property_default(const string_t& name_in);
    virtual shared_t<sink_t> add_input(const string_t& type_in, const string_t& name_in);
    virtual shared_t<source_t> add_output(const string_t& type_in, const string_t& name_in);
    virtual void add_init_args_channels();

public:
    virtual shared_t<project_t> get_project();
};

} //namespace jackalope
