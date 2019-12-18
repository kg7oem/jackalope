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

#include <jackalope/library.h>
#include <jackalope/object.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class domain_t;
class node_t;

using node_library_t = library_t<node_t, const init_list_t&>;

void add_node_constructor(const string_t& class_name_in, node_library_t::constructor_t constructor_in);

class node_t : public base_t, public object_t {

protected:
    weak_t<domain_t> domain;

public:
    static shared_t<node_t> make(const init_list_t& init_list_in);
    node_t(const init_list_t& init_list_in);
    void set_domain(shared_t<domain_t> domain_in);
    shared_t<domain_t> get_domain();
};

} // namespace jackalope
