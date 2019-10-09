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

namespace component {

base_t::~base_t()
{
    for(auto&& i : inputs) {
        delete i.second;
    }

    for(auto&& i : outputs) {
        delete i.second;
    }
}

const string_t base_t::extract_component_name(const string_t& type_in) noexcept
{
    auto extra_at = type_in.find('[');

    if (extra_at == string_t::npos) {
        return type_in;
    }

    return type_in.substr(0, extra_at);
}

const string_t base_t::extract_component_extra(const string_t& type_in) noexcept
{
    auto extra_start_char_at = type_in.find('[');
    auto extra_end_char_at = type_in.find(']');

    if (extra_start_char_at == string_t::npos) {
        return "";
    }

    if (extra_end_char_at == string_t::npos) {
        return "";
    }

    return type_in.substr(extra_start_char_at + 1, extra_end_char_at - extra_start_char_at - 1);
}

input_t::input_t(const string_t& name_in, base_t& parent_in)
: parent(parent_in), name(name_in)
{ }

output_t::output_t(const string_t& name_in, base_t& parent_in)
: parent(parent_in), name(name_in)
{ }

} // namespace component

} // namespace jackalope
