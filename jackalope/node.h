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

#include <jackalope/component.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class node : public baseobj, public lockable {

public:
    pool_map_t<string_type, component *> components;

    virtual ~node();

    template <class T, typename... Args>
    component& add_component(Args... args)
    {
        auto new_component = new T(args...);
        auto&& new_component_type = new_component->get_type();

        if (components.find(new_component_type) != components.end()) {
            delete new_component;
            throw runtime_error_t("duplicate component name");
        }

        components[new_component_type] = dynamic_cast<component *>(new_component);

        return *new_component;
    }

    component& get_component(const string_type& type_in);
    component::input& add_input(const string_type& type_in, const string_type& name_in);
};

} // namespace jackalope
