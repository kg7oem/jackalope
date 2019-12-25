// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/object.h>

namespace jackalope {

static auto object_library = new object_library_t;

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in)
{
    object_library->add_constructor(class_name_in, constructor_in);
}

shared_t<object_t> object_t::make(const init_list_t& init_list_in)
{
    if (! init_list_has("object.class", init_list_in)) {
        throw_runtime_error("missing required init arg: ", "object.class");
    }

    auto object_class = init_list_get("object.class", init_list_in);
    auto constructor = object_library->get_constructor(object_class);
    auto new_object = constructor(init_list_in);
    auto new_object_lock = new_object->get_object_lock();

    new_object->init();

    return new_object;
}

object_t::object_t(const init_list_t& init_list_in)
: init_args(init_list_in)
{ }

void object_t::init()
{
    assert_lockable_owner();

    add_slot("object.stop", [this] (shared_t<signal_t> signal_in) {
        object_stop_handler(signal_in);
    });
}

void object_t::object_stop_handler(shared_t<signal_t>)
{
    auto lock = get_object_lock();

    stop();
}

void object_t::activate()
{
    assert_lockable_owner();
}

void object_t::start()
{
    assert_lockable_owner();

    if (! stop_flag) {
        throw_runtime_error("start called when object had already been started");
    }

    stop_flag = false;
}

void object_t::stop()
{
    assert_lockable_owner();
    auto stop_lock = std::unique_lock<std::mutex>(stop_mutex);

    if (stop_flag) {
        throw_runtime_error("stop called when object had not been started");
    }

    stop_flag = true;
    stop_condition.notify_all();
}

void object_t::wait_stop()
{
    auto stop_lock = std::unique_lock<std::mutex>(stop_mutex);

    stop_condition.wait(stop_lock, [this] {
        return stop_flag;
    });
}

void object_t::set_graph(shared_t<graph_t> graph_in)
{
    assert_lockable_owner();

    graph = graph_in;
}

shared_t<graph_t> object_t::get_graph()
{
    assert_lockable_owner();

    auto strong_graph = graph.lock();

    assert(strong_graph != nullptr);

    return strong_graph;
}

shared_t<signal_t> object_t::add_signal(const string_t& name_in)
{
    assert_lockable_owner();

    if (signals.find(name_in) != signals.end()) {
        throw_runtime_error("Duplicate signal name: ", name_in);
    }

    auto signal = jackalope::make_shared<signal_t>(name_in);
    signals.insert({ name_in, signal });

    return signal;
}

shared_t<signal_t> object_t::get_signal(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = signals.find(name_in);

    if (found == signals.end()) {
        throw_runtime_error("Could not find a signal: ", name_in);
    }

    return found->second;
}

shared_t<slot_t> object_t::add_slot(const string_t& name_in, slot_handler_t handler_in)
{
    assert_lockable_owner();

    if (slots.find(name_in) != slots.end()) {
        throw_runtime_error("Duplicate slot name: ", name_in);
    }

    auto new_slot = jackalope::make_shared<slot_t>(name_in, handler_in);
    slots.insert({ name_in, new_slot });

    return new_slot;
}

shared_t<slot_t> object_t::get_slot(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = slots.find(name_in);

    if (found == slots.end()) {
        throw_runtime_error("could not find a slot: ", name_in);
    }

    return found->second;
}

shared_t<source_t> object_t::add_source(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sources.find(name_in) != sources.end()) {
        throw_runtime_error("Attempt to add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in, shared_obj());
    source->activate();
    sources[name_in] = source;

    return source;
}

shared_t<source_t> object_t::get_source(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = sources.find(name_in);

    if (found == sources.end()) {
        throw_runtime_error("Unknown source name: ", name_in);
    }

    return found->second;
}

shared_t<sink_t> object_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    assert_lockable_owner();

    if (sinks.find(name_in) != sinks.end()) {
        throw_runtime_error("Attempt to add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in, shared_obj());
    sink->activate();
    sinks[name_in] = sink;

    return sink;
}

shared_t<sink_t> object_t::get_sink(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = sinks.find(name_in);

    if (found == sinks.end()) {
        throw_runtime_error("Unknown sink name: ", name_in);
    }

    return found->second;
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto source = get_source(source_name_in);
    auto sink = target_in->get_sink(sink_name_in);

    source->link(sink);
}

} // namespace jackalope
