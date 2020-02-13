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

#include <jackalope/object.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

class project_t : public object_t {

private:
    mutex_t variables_mutex;

protected:
    pool_map_t<string_t, string_t> variables_map;

public:
    static const string_t type;

    template <class T = project_t, typename... Args>
    static shared_t<T> make(Args... args_in)
    {
        auto project = jackalope::make_shared<project_t>(args_in...);
        guard_object(project, { project->init(); });
        return project;
    }

    project_t(const init_args_t& init_args_in = init_args_t());
    const string_t& get_type();
    virtual void add_variable(const string_t& name_in, const string_t& value_in);
    virtual const string_t& get_variable(const string_t& name_in);
};

} //namespace jackalope
