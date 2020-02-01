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

#include <jackalope/async.h>
#include <jackalope/graph.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

const string_t link_available_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE;

link_available_message_t::link_available_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE, link_in)
{
    assert(link_in != nullptr);
}

const string_t link_ready_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_READY;

link_ready_message_t::link_ready_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_READY, link_in)
{
    assert(link_in != nullptr);
}

node_t::node_t(const init_args_t init_args_in)
: object_t(init_args_in), name(init_args_get(JACKALOPE_PROPERTY_NODE_NAME, init_args))
{
    assert(name != "");
}

node_t::~node_t()
{
    log_info("Destroying node: ", name);
}

void node_t::set_undef_property(const string_t& name_in)
{
    assert_lockable_owner();

    auto property = get_property(name_in);

    if (property->is_defined()) {
        return;
    }

    auto graph = get_graph();

    if (! graph->has_property(name_in)) {
        return;
    }

    auto graph_property = graph->get_property(name_in);

    if (graph_property->is_defined()) {
        property->set(graph_property->get());
    }
}

shared_t<graph_t> node_t::get_graph()
{
    assert_lockable_owner();

    auto shared = graph.lock();

    assert(shared != nullptr);

    return shared;
}

void node_t::set_graph(shared_t<graph_t> graph_in)
{
    assert_lockable_owner();

    assert(graph_in != nullptr);

    graph = graph_in;
}

shared_t<source_t> node_t::add_source(const string_t& source_name_in, const string_t& type_in)
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

shared_t<source_t> node_t::get_source(const string_t& source_name_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found == sources_by_name.end()) {
        throw_runtime_error("Unknown source name: ", source_name_in);
    }

    return found->second;
}

shared_t<source_t> node_t::get_source(const size_t source_num_in)
{
    assert_lockable_owner();

    if (source_num_in >= sources.size()) {
        throw_runtime_error("Source number is out of bounds: ", source_num_in);
    }

    return sources[source_num_in];
}

size_t node_t::get_num_sources()
{
    assert_lockable_owner();

    return sources.size();
}

shared_t<sink_t> node_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
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

shared_t<sink_t> node_t::get_sink(const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto found = sinks_by_name.find(sink_name_in);

    if (found == sinks_by_name.end()) {
        throw_runtime_error("Unknown sink name: ", sink_name_in);
    }

    return found->second;
}

shared_t<sink_t> node_t::get_sink(const size_t sink_num_in)
{
    assert_lockable_owner();

    if (sink_num_in >= sinks.size()) {
        throw_runtime_error("Sink number is out of bounds: ", sink_num_in);
    }

    return sinks[sink_num_in];
}

size_t node_t::get_num_sinks()
{
    assert_lockable_owner();

    return sinks.size();
}

void node_t::link(const string_t& source_name_in, shared_t<node_t> target_node_in, const string_t& target_sink_name_in)
{
    assert_lockable_owner();

    auto target_sink = target_node_in->get_sink(target_sink_name_in);
    auto source = get_source(source_name_in);

    source->link(target_sink);
}

void node_t::init()
{
    assert_lockable_owner();

    object_t::init();

    add_message_handler<link_ready_message_t>([this] (shared_t<link_t> link_in) { this->message_link_ready(link_in); });
    add_message_handler<link_available_message_t>([this] (shared_t<link_t> link_in) { this->message_link_available(link_in); });
}

void node_t::activate()
{
    assert_lockable_owner();

    assert(init_flag);
    assert(! activated_flag);

    if (graph.expired()) {
        throw_runtime_error("node graph weak pointer was expired when activating node");
    }

    activated_flag = true;

    add_property(JACKALOPE_PROPERTY_NODE_NAME, property_t::type_t::string, init_args);
}

void node_t::start()
{
    assert_lockable_owner();

    NODE_LOG(info, "Starting node");

    object_t::start();

    for(auto i : sources) {
        i->start();
    }

    for(auto i : sinks) {
        i->start();
    }

    NODE_LOG(info, "Done starting node");
}

void node_t::stop()
{
    assert_lockable_owner();

    object_t::stop();
}

void node_t::message_link_available(shared_t<link_t> link_in) {
    assert_lockable_owner();

    auto source = link_in->get_from();
    assert(source->get_parent() == shared_obj());

    if (! link_in->is_available()) {
        return;
    }

    source->link_available(link_in);
}

void node_t::message_link_ready(shared_t<link_t> link_in)
{
    assert_lockable_owner();

    auto sink = link_in->get_to();
    assert(sink->get_parent() == shared_obj());

    if (! link_in->is_ready()) {
        return;
    }

    sink->link_ready(link_in);
}

} //namespace jackalope
