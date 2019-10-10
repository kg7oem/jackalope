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

#include <jackalope/channel.h>
#include <jackalope/exception.h>

namespace jackalope {

static pool_map_t<string_t, input_constructor_t> input_constructors;

const string_t extract_channel_class(const string_t& class_in)
{
    auto type_start_char_at = class_in.find('[');

    if (type_start_char_at == string_t::npos) {
        return class_in;
    }

    return class_in.substr(0, type_start_char_at);
}

void add_input_constructor(const string_t& class_in, input_constructor_t constructor_in)
{
    if (input_constructors.find(class_in) != input_constructors.end()) {
        throw_runtime_error("constructor already exists for channel class: ", class_in);
    }

    input_constructors[class_in] = constructor_in;
}

input_interface_t * make_input_channel(const string_t& class_in, const string_t& name_in, node_t& parent_in)
{
    auto class_only = extract_channel_class(class_in);
    auto found = input_constructors.find(class_only);

    if (found == input_constructors.end()) {
        throw_runtime_error("unknown input channel class: ", class_only);
    }

    return found->second(name_in, parent_in);
}

channel_interface_t::channel_interface_t(const string_t& name_in, node_t& parent_in)
: name(name_in), parent(parent_in)
{ }

input_interface_t::input_interface_t(const string_t& name_in, node_t& parent_in)
: channel_interface_t(name_in, parent_in)
{ }

} // namespace jackalope
