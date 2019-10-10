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

#include <jackalope/node.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

struct input_interface_t;
struct output_interface_t;

using input_constructor_t = function_t<input_interface_t * (const string_t& name_in, node_t& parent_in)>;
using output_constructor_t = function_t<output_interface_t * (const string_t& name_in, node_t& parent_in)>;

struct channel_interface_t {
    const string_t name;
    node_t& parent;

    channel_interface_t(const string_t& name_in, node_t& parent_in);
    virtual node_t& get_parent() noexcept = 0;
    virtual const string_t& get_name() noexcept = 0;
};

struct input_interface_t : public channel_interface_t {
    input_interface_t(const string_t& name_in, node_t& parent_in);
    virtual ~input_interface_t() = default;
};

struct output_interface_t : public channel_interface_t {
    virtual ~output_interface_t() = default;
};

// example channel classes and classes with types
//   midi
//   pcm[real]
//   pcm[quad]
//   bitmap
const string_t extract_channel_class(const string_t& class_in);
const string_t extract_channel_type(const string_t& class_in);

void add_input_constructor(const string_t& class_in, input_constructor_t constructor_in);
void add_output_constructor(const string_t& class_in, output_constructor_t constructor_in);
input_interface_t * make_input_channel(const string_t& class_in, const string_t& name_in, node_t& parent_in);
output_interface_t * make_output_channel(const string_t& class_in, const string_t& name_in, node_t& parent_in);

} // namespace jackalope
