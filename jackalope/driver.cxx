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

#include <jackalope/domain.h>
#include <jackalope/driver.h>
#include <jackalope/exception.h>

namespace jackalope {

static pool_map_t<string_t, driver_constructor_t> node_constructors;

void add_driver_constructor(const string_t& class_name_in, driver_constructor_t constructor_in)
{
    if (node_constructors.find(class_name_in) != node_constructors.end()) {
        throw_runtime_error("Can not add duplicate node constructor for class: ", class_name_in);
    }

    node_constructors[class_name_in] = constructor_in;
}

driver_constructor_t get_driver_constructor(const string_t& class_name_in)
{
    auto found = node_constructors.find(class_name_in);

    if (found == node_constructors.end()) {
        throw_runtime_error("could not find node constructor for class: ", class_name_in);
    }

    return found->second;
}

shared_t<driver_t> driver_t::make(const init_list_t& init_list_in)
{
    if (! init_list_has("driver:class", init_list_in)) {
        throw_runtime_error("missing driver class in arguments");
    }

    auto driver_class = init_list_get("driver:class", init_list_in);
    auto driver_constructor = get_driver_constructor(driver_class);

    auto driver = driver_constructor(init_list_in);
    driver->init();

    return driver;
}

driver_t::driver_t(const init_list_t& init_list_in)
: init_args(init_list_in)
{ }

void driver_t::set_domain(shared_t<domain_t> domain_in)
{
    domain = domain_in;
}

shared_t<domain_t> driver_t::get_domain__e()
{
    assert_lockable_owner();
    assert(! domain.expired());

    return domain.lock();
}

void driver_t::init()
{
    auto lock = get_object_lock();
    return init__e();
}

void driver_t::init__e()
{
    assert_lockable_owner();
}

void driver_t::activate()
{
    auto lock = get_object_lock();
    return activate__e();
}

void driver_t::activate__e()
{
    assert_lockable_owner();
}

void driver_t::start()
{
    auto lock = get_object_lock();
    return start__e();
}

void driver_t::start__e()
{
    assert_lockable_owner();
}

void driver_t::stop()
{
    auto lock = get_object_lock();
    return stop__e();
}

void driver_t::stop__e()
{
    assert_lockable_owner();
}

} // namespace jackalope
