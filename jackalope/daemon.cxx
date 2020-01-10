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

#include <jackalope/daemon.h>

namespace jackalope {

static daemon_library_t * daemon_library = new daemon_library_t();

void add_daemon_constructor(const string_t& class_name_in, daemon_library_t::constructor_t constructor_in)
{
    daemon_library->add_constructor(class_name_in, constructor_in);
}

shared_t<daemon_t> daemon_t::make(const string_t& type_in, const init_args_t& init_args_in)
{
    auto constructor = daemon_library->get_constructor(type_in);

    auto daemon = constructor(type_in, init_args_in);
    auto daemon_lock = daemon->get_object_lock();
    daemon->init();

    return daemon;
}

daemon_t::daemon_t(const string_t& type_in, const init_args_t& init_args_in)
: type(type_in), init_args(init_args_in)
{ }

daemon_t::~daemon_t()
{
    assert(stopped_flag);
}

void daemon_t::init()
{
    assert_lockable_owner();

    assert(! initialized_flag);

    initialized_flag = true;
}

void daemon_t::start()
{
    assert_lockable_owner();

    assert(! started_flag);
    assert(initialized_flag);

    started_flag = true;
}

void daemon_t::stop()
{
    assert_lockable_owner();

    assert(! stopped_flag);

    if (! started_flag) {
        return;
    }

    stopped_flag = true;
}

} //namespace jackalope
