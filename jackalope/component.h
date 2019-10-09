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

#include <jackalope/component.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

namespace component {

class input_t : public baseobj_t, public lockable_t {

protected:
    base_t& parent;

public:
    const string_t name;

    input_t(const string_t& name_in, base_t& parent_in);
    virtual const string_t& get_type() = 0;
};

class output_t : public baseobj_t, public lockable_t {
protected:
    base_t& parent;

public:
    const string_t name;

    output_t(const string_t& name_in, base_t& parent_in);
    virtual const string_t& get_type() = 0;
};

struct base_t : public baseobj_t, public lockable_t {
    pool_map_t<string_t, input_t *> inputs;
    pool_map_t<string_t, output_t *> outputs;

    virtual ~base_t();
    static const string_t extract_component_name(const string_t& type_in) noexcept;
    static const string_t extract_component_extra(const string_t& type_in) noexcept;
    virtual const string_t& get_type() = 0;
    virtual input_t& add_input(const string_t& type_in, const string_t& name_in) = 0;
};

} // namespace component

} // namespace jackalope
