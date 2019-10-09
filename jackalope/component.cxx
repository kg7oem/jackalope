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

#include <jackalope/component.h>

namespace jackalope {

component::~component()
{
    for(auto&& i : inputs) {
        delete i.second;
    }

    for(auto&& i : outputs) {
        delete i.second;
    }
}

const string_type component::extract_component_name(const string_type& type_in) noexcept
{
    auto extra_at = type_in.find('[');

    if (extra_at == string_type::npos) {
        return type_in;
    }

    return type_in.substr(0, extra_at);
}

const string_type component::extract_component_extra(const string_type& type_in) noexcept
{
    auto extra_start_char_at = type_in.find('[');
    auto extra_end_char_at = type_in.find(']');

    if (extra_start_char_at == string_type::npos) {
        return "";
    }

    if (extra_end_char_at == string_type::npos) {
        return "";
    }

    return type_in.substr(extra_start_char_at + 1, extra_end_char_at - extra_start_char_at - 1);
}

component::input::input(const string_type& name_in, component& parent_in)
: parent(parent_in), name(name_in)
{ }

component::output::output(const string_type& name_in, component& parent_in)
: parent(parent_in), name(name_in)
{ }

} // namespace jackalope
