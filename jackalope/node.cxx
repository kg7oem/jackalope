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

node::~node()
{
    for(auto&& i : components) {
        delete i.second;
    }
}

component_t& node::get_component(const string_t& type_in)
{
    auto found = components.find(type_in);

    if (found == components.end()) {
        throw_runtime_error("unknown component type: ", type_in);
    }

    return *found->second;
}

component_t::input_t& node::add_input(const string_t& type_in, const string_t& name_in)
{
    auto component_name = component_t::extract_component_name(type_in);
    auto& component = get_component(component_name);

    return component.add_input(type_in, name_in);
}

} // namespace jackalope
