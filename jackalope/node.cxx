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

#define INPUT_PREFIX "input:"
#define OUTPUT_PREFIX "output:"

namespace jackalope {

node_t::node_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: object_t(init_args_in), weak_project(project_in)
{ }

void node_t::did_stop()
{
    assert_lockable_owner();

    get_project()->send_message<project_node_stopped_message_t>(shared_obj<node_t>());
}

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

shared_t<sink_t> node_t::add_input(const string_t& type_in, const string_t& name_in)
{
    assert_lockable_owner();

    return add_sink(type_in, to_string(INPUT_PREFIX, name_in));
}

shared_t<source_t> node_t::add_output(const string_t& type_in, const string_t& name_in)
{
    assert_lockable_owner();

    return add_source(type_in, to_string(OUTPUT_PREFIX, name_in));
}

} //namespace jackalope
