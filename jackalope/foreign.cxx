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

#include <iostream>

#include <jackalope/async.h>
#include <jackalope/foreign.h>
#include <jackalope/jackalope.h>

using namespace jackalope;

jackalope_source_t::jackalope_source_t(shared_t<jackalope::source_t> wrapped_in)
: jackalope_wrapper_t(wrapped_in)
{ }

jackalope_sink_t::jackalope_sink_t(shared_t<jackalope::sink_t> wrapped_in)
: jackalope_wrapper_t(wrapped_in)
{ }

jackalope_object_t::jackalope_object_t(jackalope::shared_t<jackalope::object_t> wrapped_in)
: jackalope_wrapper_t(wrapped_in)
{
    assert(wrapped_in != nullptr);
}

void jackalope_object_t::activate()
{
    wait_job<void>([&] {
        auto lock = wrapped->get_object_lock();
        wrapped->activate();
    });
}

void jackalope_object_t::start()
{
    wait_job<void>([&] {
        auto lock = wrapped->get_object_lock();
        wrapped->start();
    });
}

void jackalope_object_t::stop()
{
    wait_job<void>([&] {
        auto lock = wrapped->get_object_lock();
        wrapped->stop();
    });
}

jackalope_source_t jackalope_object_t::add_source(const string_t& name_in, const string_t& type_in)
{
    auto new_source = wait_job<shared_t<jackalope::source_t>>([&] {
        auto lock = wrapped->get_object_lock();
        return wrapped->add_source(name_in, type_in);
    });

    return jackalope_source_t(new_source);
}

jackalope_sink_t jackalope_object_t::add_sink(const string_t& name_in, const string_t& type_in)
{
    auto new_sink = wait_job<shared_t<jackalope::sink_t>>([&] {
        auto lock = wrapped->get_object_lock();
        return wrapped->add_sink(name_in, type_in);
    });

    return jackalope_sink_t(new_sink);
}

void jackalope_object_t::link(const jackalope::string_t& source_name_in, jackalope_object_t& target_object_in, const jackalope::string_t& target_sink_name_in)
{
    wait_job<void>([this, source_name_in, &target_object_in, target_sink_name_in] {
        auto source_lock = wrapped->get_object_lock();
        auto source = wrapped->get_source(source_name_in);
        auto target = target_object_in.wrapped;
        auto target_lock = target->get_object_lock();
        auto target_sink = target->get_sink(target_sink_name_in);

        source->link(target_sink);
    });
}

void jackalope_object_t::connect(const string_t& signal_name_in, jackalope_object_t& target_object_in, const string_t& slot_name_in)
{
    wait_job<void>([&] {
        auto from_lock = wrapped->get_object_lock();
        auto to_lock = target_object_in.wrapped->get_object_lock();

        auto signal = wrapped->get_signal(signal_name_in);
        auto slot = target_object_in.wrapped->get_slot(slot_name_in);

        signal->subscribe(slot);
    });
}

jackalope_graph_t jackalope_graph_t::make(const jackalope::init_args_t& init_args_in)
{
    auto new_graph = jackalope::graph_t::make(init_args_in);
    return jackalope_graph_t(new_graph);
}

jackalope_graph_t::jackalope_graph_t(shared_t<graph_t> wrapped_in)
: jackalope_object_t(wrapped_in)
{ }

jackalope_node_t jackalope_graph_t::add_node(const init_args_t& init_args_in)
{
    auto graph = dynamic_pointer_cast<jackalope::graph_t>(wrapped);

    auto new_node = wait_job<shared_t<jackalope::node_t>>([&] {
        auto lock = graph->get_object_lock();
        return graph->add_node(init_args_in);
    });

    return jackalope_node_t(new_node);
}

void jackalope_graph_t::run()
{
    auto stopped_signal = wait_job<shared_t<signal_t>>([&] {
        auto lock = wrapped->get_object_lock();
        auto signal = wrapped->get_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED);

        wrapped->start();
        return signal;
    });

    stopped_signal->wait();
}

jackalope_node_t jackalope_node_t::make(const init_args_t& init_args_in)
{
    auto new_node = jackalope::object_t::make<jackalope::node_t>(init_args_in);
    return jackalope_node_t(new_node);
}

jackalope_node_t::jackalope_node_t(shared_t<jackalope::node_t> wrapped_in)
: jackalope_object_t(wrapped_in)
{ }

void jackalope_node_t::run()
{
    auto node = dynamic_pointer_cast<jackalope::node_t>(wrapped);

    wait_job<void>([&] {
        auto lock = node->get_object_lock();
        node->run();
    });
}

extern "C" {

void jackalope_init()
{
    jackalope::init();
}

void jackalope_shutdown()
{
    jackalope::shutdown();
}

static init_args_t init_args_from_strings(const char ** strings_in)
{
    init_args_t init_args;

    for(const char ** string = strings_in; *string != nullptr; string += 2) {
        auto key = *string;
        auto value = *(string + 1);

        assert(key != nullptr);
        assert(value != nullptr);

        init_args.push_back({ key, value });
    }

    return init_args;
}

void jackalope_object_delete(jackalope_object_t * object_in)
{
    assert(object_in != nullptr);

    delete object_in;
}

struct jackalope_source_t * jackalope_object_add_source(jackalope_object_t * object_in, const char * type_in, const char * name_in)
{
    assert(object_in != nullptr);

    auto new_source = object_in->add_source(type_in, name_in);
    auto wrapped_source = new_source.wrapped;
    return new jackalope_source_t(wrapped_source);
}

struct jackalope_sink_t * jackalope_object_add_sink(jackalope_object_t * object_in, const char * type_in, const char * name_in)
{
    assert(object_in != nullptr);

    auto new_sink = object_in->add_sink(type_in, name_in);
    auto wrapped_sink = new_sink.wrapped;
    return new jackalope_sink_t(wrapped_sink);
}

void jackalope_object_connect(jackalope_object_t * object_in, const char * signal_in, jackalope_object_t * target_object_in, const char * slot_in)
{
    assert(object_in != nullptr);

    object_in->connect(signal_in, *target_object_in, slot_in);
}

void jackalope_object_link(jackalope_object_t * object_in, const char * source_in, jackalope_object_t * target_object_in, const char * sink_in)
{
    assert(object_in != nullptr);

    object_in->link(source_in, *target_object_in, sink_in);
}

void jackalope_object_start(jackalope_object_t * object_in)
{
    assert(object_in != nullptr);

    object_in->start();
}

void jackalope_object_stop(jackalope_object_t * object_in)
{
    assert(object_in != nullptr);

    object_in->stop();
}

struct jackalope_object_t * jackalope_graph_make(const char * init_args_in[])
{
    auto init_args = init_args_from_strings(init_args_in);
    auto new_graph = graph_t::make(init_args);
    return new jackalope_graph_t(new_graph);
}

struct jackalope_object_t * jackalope_graph_add_node(jackalope_object_t * graph_in, const char * init_args_in[])
{
    assert(graph_in != nullptr);

    auto wrapped_graph = dynamic_pointer_cast<graph_t>(graph_in->wrapped);
    auto init_args = init_args_from_strings(init_args_in);
    auto graph_lock = wrapped_graph->get_object_lock();
    auto new_node = wrapped_graph->add_node(init_args);
    return new jackalope_node_t(new_node);
}

void jackalope_graph_run(struct jackalope_object_t * graph_in)
{
    assert(graph_in != nullptr);

    auto graph = dynamic_cast<jackalope_graph_t *>(graph_in);
    graph->run();
}

struct jackalope_object_t * jackalope_node_make(const char ** init_args_in)
{
    auto init_args = init_args_from_strings(init_args_in);
    auto new_node = object_t::make<jackalope::node_t>(init_args);
    return new jackalope_node_t(new_node);
}

void jackalope_node_run(struct jackalope_object_t * object_in)
{
    assert(object_in != nullptr);

    auto node = dynamic_pointer_cast<jackalope_node_t>(object_in->wrapped);
    node->run();
}

} // extern "C"
