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

#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.h>
#include <jackalope/types.h>

namespace jackalope {

static shared_t<source_t> pcm_source_real_constructor(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<pcm_source_t<real_t>>(name_in, type_in, parent_in);
}

static shared_t<sink_t> pcm_sink_real_constructor(const string_t& name_in, const string_t& type_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<pcm_sink_t<real_t>>(name_in, type_in, parent_in);
}

void pcm_init()
{
    add_source_constructor(JACKALOPE_CHANNEL_TYPE_PCM_REAL, pcm_source_real_constructor);
    add_sink_constructor(JACKALOPE_CHANNEL_TYPE_PCM_REAL, pcm_sink_real_constructor);
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

    add_signal(JACKALOPE_SIGNAL_PCM_READY, [this] {
        auto lock = get_object_lock();
        this->pcm_ready();
    });

    for(auto& i : init_args_find("source", init_args)) {
        add_source(i.first, i.second);
    }

    for(auto& i : init_args_find("sink", init_args)) {
        add_sink(i.first, i.second);
    }
}

void pcm_node_t::activate()
{
    assert_lockable_owner();

    node_t::activate();

    init_undef_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);
    init_undef_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE);
}

void pcm_node_t::pcm_ready()
{
    assert_lockable_owner();
}

} // namespace jackalope
