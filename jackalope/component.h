// Jackalope Audio Engine
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

#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

struct component : public baseobj_t, public lockable_t {
    class input : public baseobj_t, public lockable_t {

    protected:
        component& parent;

    public:
        const string_t name;

        input(const string_t& name_in, component& parent_in);
        virtual const string_t& get_type() = 0;
    };

    class output : public baseobj_t, public lockable_t {
    protected:
        component& parent;

    public:
        const string_t name;

        output(const string_t& name_in, component& parent_in);
        virtual const string_t& get_type() = 0;
    };

    pool_map_t<string_t, input *> inputs;
    pool_map_t<string_t, output *> outputs;

    virtual ~component();
    static const string_t extract_component_name(const string_t& type_in) noexcept;
    static const string_t extract_component_extra(const string_t& type_in) noexcept;
    virtual const string_t& get_type() = 0;
    virtual input& add_input(const string_t& type_in, const string_t& name_in) = 0;
};

} // namespace jackalope
