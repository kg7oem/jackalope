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

namespace jackalope {

struct project_node_stopped_message_t : public message_t<const shared_t<node_t>> {
    inline static const string_t message_name = "project.node_stopped";
    project_node_stopped_message_t(shared_t<node_t> node_in);
};

class project_t : public object_t {

private:
    mutex_t variables_mutex;

protected:
    pool_map_t<string_t, string_t> variables_map;
    pool_list_t<shared_t<node_t>> nodes;

    virtual void will_init() override;
    virtual void did_start() override;
    virtual void will_stop() override;
    virtual void will_shutdown() override;
    virtual void message_project_node_stopped(shared_t<node_t> node_in);

public:
    inline static const string_t type = "project";

    template <class T = project_t, typename... Args>
    static shared_t<T> make(Args... args_in)
    {
        auto project = jackalope::make_shared<project_t>(args_in...);
        guard_object(project, { project->init(); project->activate(); });
        return project;
    }

    project_t(const init_args_t& init_args_in = init_args_t());
    ~project_t();
    virtual const string_t& get_type();
    virtual void add_variable(const string_t& name_in, const string_t& value_in);
    virtual bool has_variable(const string_t& name_in);
    virtual const string_t& get_variable(const string_t& name_in);
    shared_t<node_t> make_node(const init_args_t& init_args_in);
};

} //namespace jackalope
