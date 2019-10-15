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

#include <jackalope/channel.h>
#include <jackalope/node.forward.h>
#include <jackalope/property.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class node_t : public baseobj_t {

protected:
    friend void input_t::notify();

    const string_t name;
    pool_map_t<string_t, property_t> properties;
    pool_map_t<string_t, input_t *> inputs;
    pool_map_t<string_t, output_t *> outputs;

    node_t(const string_t& name_in);
    virtual ~node_t();
    virtual const string_t& get_name();
    virtual property_t& add_property(const string_t& name_in, property_t::type_t type_in);
    property_t& get_property(const string_t& name_in);
    virtual input_t& add_input(const string_t& channel_class_in, const string_t& name_in);
    // virtual input_t& get_input(const string_t& name_in);
    virtual output_t& add_output(const string_t& channel_class_in, const string_t& name_in);
    // virtual output_t& get_output(const string_t& name_in);
    virtual void input_ready(input_t& input_in) = 0;
};

} // namespace jackalope
