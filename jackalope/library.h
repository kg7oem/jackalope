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

#include <jackalope/exception.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

template <class T, class... Args>
class library_t : public base_obj_t, public lock_obj_t {

public:
    using constructor_t = function_t<shared_t<T> (Args...)>;

protected:
    pool_map_t<string_t, constructor_t> constructors;

    void _add_constructor(const string_t& name_in, constructor_t constructor_in)
    {
        assert_lockable_owner();

        auto found = constructors.find(name_in);

        if (found != constructors.end()) {
            throw_runtime_error("Duplicate constructor for type: ", name_in);
        }

        constructors.emplace(name_in, constructor_in);
    }

    constructor_t _get_constructor(const string_t& name_in)
    {
        assert_lockable_owner();

        auto found = constructors.find(name_in);

        if (found == constructors.end()) {
            throw_runtime_error("Unknown constructor for type: ", name_in);
        }

        return found->second;
    }


public:
    void add_constructor(const string_t& name_in, constructor_t constructor_in)
    {
        auto lock = get_object_lock();
        _add_constructor(name_in, constructor_in);
    }

    constructor_t get_constructor(const string_t& name_in)
    {
        auto lock = get_object_lock();
        return _get_constructor(name_in);
    }
};

} // namespace jackalope
