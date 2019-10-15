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

#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

node_t::node_t(const string_t& name_in)
: name(name_in)
{ }

node_t::~node_t()
{
    for (auto i : inputs) {
        delete i.second;
    }

    for (auto i : outputs) {
        delete i.second;
    }
}

const string_t& node_t::get_name()
{
    return name;
}

input_t& node_t::add_input(const string_t& channel_class_in, const string_t& name_in)
{
    if (inputs.find(channel_class_in) != inputs.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto new_channel = make_input_channel(channel_class_in, name_in, *this);
    inputs[name_in] = new_channel;

    return *new_channel;
}

output_t& node_t::add_output(const string_t& channel_class_in, const string_t& name_in)
{
    if (inputs.find(channel_class_in) != inputs.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto new_channel = make_output_channel(channel_class_in, name_in, *this);
    outputs[name_in] = new_channel;

    return *new_channel;
}

} // namespace jackalope
