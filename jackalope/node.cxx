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

#include <jackalope/node.h>
#include <jackalope/project.h>

namespace jackalope {

node_t::node_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: object_t(init_args_in), weak_project(project_in)
{ }

shared_t<project_t> node_t::get_project()
{
    assert_lockable_owner();

    return weak_project.lock();
}

std::pair<bool, string_t> node_t::get_property_default(const string_t& name_in)
{
    assert_lockable_owner();

    auto default_value = object_t::get_property_default(name_in);

    if (default_value.first) {
        return default_value;
    }

    auto project = get_project();

    if (project->has_variable(name_in)) {
        return { true, project->get_variable(name_in) };
    }

    return { false, "" };
}

} //namespace jackalope
