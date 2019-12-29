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
#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.h>
#include <jackalope/types.h>

namespace jackalope {

static shared_t<source_t> pcm_source_real_constructor(const string_t& name_in, const string_t& type_in)
{
    return jackalope::make_shared<pcm_real_source_t>(name_in, type_in);
}

static shared_t<sink_t> pcm_sink_real_constructor(const string_t& name_in, const string_t& type_in)
{
    return jackalope::make_shared<pcm_real_sink_t>(name_in, type_in);
}

void pcm_init()
{
    add_source_constructor(JACKALOPE_CHANNEL_TYPE_PCM_REAL, pcm_source_real_constructor);
    add_sink_constructor(JACKALOPE_CHANNEL_TYPE_PCM_REAL, pcm_sink_real_constructor);
}

pcm_real_sink_t::pcm_real_sink_t(const string_t& name_in, const string_t& type_in)
: sink_t(name_in, type_in)
{ }

pcm_real_source_t::pcm_real_source_t(const string_t& name_in, const string_t& type_in)
: source_t(name_in, type_in)
{ }

void pcm_real_source_t::set_buffer(shared_t<buffer_t> buffer_in)
{
    if (buffer != nullptr) {
        throw_runtime_error("set_buffer() called when a buffer was already present");
    }

    buffer = buffer_in;
    notify();
}

void pcm_real_source_t::notify()
{
    log_info("notify() called for pcm source: ", name);

    // for(auto i : links) {
    //     auto sink = i->get_to()->shared_obj<pcm_real_sink_t>();
    //     auto source = shared_obj<pcm_real_source_t>();

    //     submit_job([source, sink] {
    //         auto lock = sink->get_parent()->get_object_lock();
    //         sink->source_ready(source);
    //     });
    // }
}

void pcm_real_source_t::link(shared_t<sink_t> sink_in)
{
    if (type != sink_in->type) {
        throw_runtime_error("incompatible types during link: ", type, " != ", sink_in->type);
    }

    source_t::link(sink_in);
}

pcm_node_t::pcm_node_t(const init_list_t& init_list_in)
: node_t(init_list_in)
{ }

void pcm_node_t::init()
{
    node_t::init();

    assert_lockable_owner();

    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);

    add_signal(JACKALOPE_SIGNAL_PCM_SINKS_READY, [this] {
        auto lock = get_object_lock();
        this->pcm_sinks_ready();
    });

    add_signal(JACKALOPE_SIGNAL_PCM_SOURCES_READY, [this] {
        auto lock = get_object_lock();
        this->pcm_sources_ready();
    });

    for(auto& i : init_args_find("source", init_args)) {
        auto parts = split_string(i.first, '.');
        auto name = parts[1];
        add_source(name, i.second);
    }

    for(auto& i : init_args_find("sink", init_args)) {
        auto parts = split_string(i.first, '.');
        auto name = parts[1];
        add_sink(name, i.second);
    }
}

void pcm_node_t::activate()
{
    assert_lockable_owner();

    node_t::activate();

    init_undef_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);
    init_undef_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE);
}

void pcm_node_t::reset()
{
    node_t::reset();

    assert_lockable_owner();

    reset_pcm_sinks();
    reset_pcm_sources();
}

void pcm_node_t::reset_pcm_sinks()
{
    assert_lockable_owner();
}

void pcm_node_t::reset_pcm_sources()
{
    assert_lockable_owner();
}

void pcm_node_t::pcm_sinks_ready()
{
    assert_lockable_owner();
}

void pcm_node_t::pcm_sources_ready()
{
    assert_lockable_owner();
}

} // namespace jackalope
