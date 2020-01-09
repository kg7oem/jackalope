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

#include <jackalope/daemon.forward.h>
#include <jackalope/library.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

using daemon_library_t = library_t<daemon_t, const string_t&, const init_args_t&>;

void add_daemon_constructor(const string_t& class_name_in, daemon_library_t::constructor_t constructor_in);

struct daemon_t : public shared_obj_t<daemon_t>, public lockable_t {

protected:
    daemon_t(const string_t& type_in, const init_args_t& init_args_in);

public:
    const string_t type;
    const init_args_t init_args;

    static shared_t<daemon_t> make(const string_t& type_in, const init_args_t& init_args_in);
    virtual ~daemon_t() = default;
    virtual void init();
    virtual void start();
};

} //namespace jackalope
