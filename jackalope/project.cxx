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

#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/module.h>
#include <jackalope/node.h>
#include <jackalope/plugin.h>
#include <jackalope/project.h>

namespace jackalope {

project_node_stopped_message_t::project_node_stopped_message_t(shared_t<node_t> node_in)
: message_t(project_node_stopped_message_t::message_name, node_in)
{
    assert(node_in != nullptr);
}

project_t::project_t(const init_args_t& init_args_in)
: object_t(init_args_in)
{
    for(auto i : init_args_in) {
        add_variable(i.first, i.second);
    }
}

void project_t::will_init()
{
    assert_lockable_owner();

    add_message_handler<project_node_stopped_message_t>([this] (shared_t<node_t> node_in) { message_project_node_stopped(node_in); });

    object_t::will_init();
}

void project_t::did_start()
{
    assert_lockable_owner();
    assert(running_flag);

    for(auto i : nodes) {
        i->post_slot(JACKALOPE_SLOT_OBJECT_START);
    }
}

void project_t::will_stop()
{
    assert_lockable_owner();
    assert(running_flag);

    object_log_trace("stopping all owned nodes");

    for(auto i : nodes) {
        guard_object(i, { i->stop(); });
    }

    object_t::will_stop();
}

void project_t::will_shutdown()
{
    assert_lockable_owner();
    assert(! shutdown_flag);

    for (auto i : nodes) {
        guard_object(i, { i->shutdown(); });
    }

    nodes.empty();
}

void project_t::message_project_node_stopped(shared_t<node_t> node_in)
{
    assert_lockable_owner();

    object_log_info("owned node stopped: ", node_in->description());
}

const string_t& project_t::get_type()
{
    return type;
}

void project_t::add_variable(const string_t& name_in, const string_t& value_in)
{
    lock_t variables_lock(variables_mutex);

    if (variables_map.count(name_in) > 0) {
        throw_runtime_error("can not add duplicate variable: ", name_in);
    }

    variables_map[name_in] = value_in;
}

bool project_t::has_variable(const string_t& name_in)
{
    lock_t variables_lock(variables_mutex);

    return variables_map.count(name_in) != 0;
}

const string_t& project_t::get_variable(const string_t& name_in)
{
    lock_t variables_lock(variables_mutex);

    if (variables_map.count(name_in) == 0) {
        throw_runtime_error("variable does not exist: ", name_in);
    }

    return variables_map[name_in];
}

shared_t<node_t> project_t::make_plugin(const init_args_t& init_args_in)
{
    assert_lockable_owner();

    if (! init_args_has(JACKALOPE_PROPERTY_NODE_TYPE, init_args_in)) {
        throw_runtime_error("missing argument: ", JACKALOPE_PROPERTY_NODE_TYPE);
    }

    auto type = init_args_get(JACKALOPE_PROPERTY_NODE_TYPE, init_args_in);
    auto constructor = get_plugin_constructor(type);
    auto node = constructor(shared_obj<project_t>(), init_args_in);

    nodes.push_back(node);

    return node;
}

} //namespace jackalope
