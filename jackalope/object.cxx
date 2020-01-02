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
#include <jackalope/object.h>

namespace jackalope {

static object_library_t * object_library = new object_library_t();

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in)
{
    object_library->add_constructor(class_name_in, constructor_in);
}

size_t _get_object_id()
{
    static atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);

    return current_id++;
}

shared_t<object_t> object_t::_make(const init_list_t init_list_in)
{
    auto init_args = init_args_from_list(init_list_in);

    if (! init_args_has(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args)) {
        throw_runtime_error("missing object type");
    }

    auto object_type = init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args);
    auto constructor = object_library->get_constructor(object_type);
    auto new_object = constructor(init_list_in);
    auto lock = new_object->get_object_lock();
    new_object->init();
    return new_object;
}

object_t::object_t(const init_list_t init_list_in)
: init_args(init_args_from_list(init_list_in)), type(init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args))
{
    assert(type != "");
}

shared_t<source_t> object_t::add_source(const string_t& source_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found != sources_by_name.end()) {
        throw_runtime_error("Can not add duplicate source name: ", source_name_in);
    }

    auto new_source = source_t::make(source_name_in, type_in, shared_obj());
    sources.push_back(new_source);
    sources_by_name[new_source->name] = new_source;

    return new_source;
}

shared_t<source_t> object_t::get_source(const string_t& source_name_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found == sources_by_name.end()) {
        throw_runtime_error("Unknown source name: ", source_name_in);
    }

    return found->second;
}

shared_t<source_t> object_t::get_source(const size_t source_num_in)
{
    assert_lockable_owner();

    if (source_num_in >= sources.size()) {
        throw_runtime_error("Source number is out of bounds: ", source_num_in);
    }

    return sources[source_num_in];
}

shared_t<sink_t> object_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto found = sinks_by_name.find(sink_name_in);

    if (found != sinks_by_name.end()) {
        throw_runtime_error("Can not add duplicate sink name: ", sink_name_in);
    }

    auto new_source = sink_t::make(sink_name_in, type_in, shared_obj());
    sinks.push_back(new_source);
    sinks_by_name[new_source->name] = new_source;

    return new_source;
}

shared_t<sink_t> object_t::get_sink(const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto found = sinks_by_name.find(sink_name_in);

    if (found == sinks_by_name.end()) {
        throw_runtime_error("Unknown sink name: ", sink_name_in);
    }

    return found->second;
}

shared_t<sink_t> object_t::get_sink(const size_t sink_num_in)
{
    assert_lockable_owner();

    if (sink_num_in >= sources.size()) {
        throw_runtime_error("Sink number is out of bounds: ", sink_num_in);
    }

    return sinks[sink_num_in];
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_object_in, const string_t& target_sink_name_in)
{
    assert_lockable_owner();

    auto target_sink = target_object_in->get_sink(target_sink_name_in);
    auto source = get_source(source_name_in);

    source->link(target_sink);
}

void object_t::init()
{
    assert_lockable_owner();
}

void object_t::start()
{
    assert_lockable_owner();

    for(auto i : sources) {
        i->start();
    }

    for(auto i : sinks) {
        i->start();
    }
}

void object_t::stop()
{
    assert_lockable_owner();

    stopped_flag = true;
}

void object_t::source_available(shared_t<source_t> source_in)
{
    assert_lockable_owner();

    log_info("source available: ", source_in->name);
}

void object_t::slot_source_available(shared_t<source_t> source_in)
{
    auto lock = get_object_lock();

    source_available(source_in);
}

void object_t::source_unavailable(shared_t<source_t> source_in)
{
    assert_lockable_owner();

    log_info("source unavailable: ", source_in->name);
}

void object_t::slot_source_unavailable(shared_t<source_t> source_in)
{
    auto lock = get_object_lock();

    source_unavailable(source_in);
}

void object_t::sink_ready(shared_t<sink_t> sink_in)
{
    assert_lockable_owner();

    log_info("sink ready: ", sink_in->name);
}

void object_t::slot_sink_ready(shared_t<sink_t> available_sink_in)
{
    auto lock = get_object_lock();

    sink_ready(available_sink_in);
}

} //namespace jackalope
