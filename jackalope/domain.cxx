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
#include <jackalope/pcm.h>

namespace jackalope {

shared_t<domain_t> domain_t::make(const init_list_t& init_list_in)
{
    auto domain = jackalope::make_shared<domain_t>(init_list_in);
    domain->init();
    return domain;
}

domain_t::domain_t(const init_list_t& init_list_in)
: init_args(init_list_in)
{
    add_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
    add_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
}

void domain_t::init()
{
    auto lock = get_object_lock();
    init__e();
}

void domain_t::init__e()
{
    assert_lockable_owner();

    if (init_list_has(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, init_args)) {
        auto value = init_list_get(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, init_args);
        get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).set(value);
    }

    if (init_list_has(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, init_args)) {
        auto value = init_list_get(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, init_args);
        get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE).set(value);
    }
}

void domain_t::run()
{
    auto lock = get_object_lock();
    run__e();
}

void domain_t::run__e()
{
    assert_lockable_owner();
}

void domain_t::stop()
{
    auto lock = get_object_lock();
    return stop__e();
}

void domain_t::stop__e()
{
    assert_lockable_owner();
}

shared_t<driver_t> domain_t::add_driver(const init_list_t& init_list_in)
{
    auto lock = get_object_lock();
    return add_driver__e(init_list_in);
}

shared_t<driver_t> domain_t::add_driver__e(const init_list_t&)
{
    assert_lockable_owner();

    return make_shared<driver_t>();
}

shared_t<node_t> domain_t::add_node(const init_list_t&)
{
    return make_shared<node_t>();
}

} // namespace jackalope
