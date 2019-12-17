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
: object_t(init_list_in)
{
    add_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, property_t::type_t::size);
    add_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, property_t::type_t::size);
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

void domain_t::run__e()
{
    assert_lockable_owner();

    for(auto i : drivers) {
        i->start();
    }

    while(1) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
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

shared_t<driver_t> domain_t::add_driver__e(const init_list_t& init_list_in)
{
    assert_lockable_owner();

    auto driver = driver_t::make(init_list_in);
    driver->set_domain(shared_obj());
    driver->activate();

    drivers.push_back(driver);

    return driver;
}

shared_t<node_t> domain_t::add_node(const init_list_t&)
{
    return make_shared<node_t>();
}

shared_t<source_t> domain_t::add_source(const string_t& name_in, const string_t& type_in)
{
    if (sources.find(name_in) != sources.end()) {
        throw_runtime_error("Attempt to add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in, shared_obj());
    source->activate();
    sources[name_in] = source;

    return source;
}

shared_t<sink_t> domain_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    if (sinks.find(name_in) != sinks.end()) {
        throw_runtime_error("Attempt to add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in, shared_obj());
    sink->activate();
    sinks[name_in] = sink;

    return sink;
}

} // namespace jackalope
