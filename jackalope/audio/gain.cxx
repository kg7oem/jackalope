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

#include <jackalope/audio.h>
#include <jackalope/audio/gain.h>

namespace jackalope {

namespace audio {

static shared_t<gain_node_t> gain_node_constructor(NDEBUG_UNUSED const string_t& type_in, const init_args_t init_args_in)
{
    assert(type_in == JACKALOPE_AUDIO_GAIN_OBJECT_TYPE);

    return jackalope::make_shared<gain_node_t>(init_args_in);
}

void gain_init()
{
    add_object_constructor(JACKALOPE_AUDIO_GAIN_OBJECT_TYPE, gain_node_constructor);
}

gain_node_t::gain_node_t(const init_args_t init_args_in)
: filter_plugin_t(init_args_in)
{ }

void gain_node_t::init()
{
    assert_lockable_owner();

    add_property("pcm.buffer_size", property_t::type_t::size, init_args);
    add_property("pcm.sample_rate", property_t::type_t::size, init_args);

    filter_plugin_t::init();
}

void gain_node_t::activate()
{
    assert_lockable_owner();

    add_property("config.gain", property_t::type_t::real, init_args);

    add_source("output", "audio");
    add_sink("input", "audio");

    filter_plugin_t::activate();
}

void gain_node_t::execute()
{
    assert_lockable_owner();

    auto scale_by = pcm_db_scale_factor(get_property("config.gain")->get_real());
    auto sink = get_sink<audio_sink_t>(0);
    auto source = get_source<audio_source_t>(0);

    auto input_buffer = sink->get_buffer();
    sink->reset();

    auto output_buffer = make_shared<audio_buffer_t>(input_buffer->num_samples);
    pcm_copy(input_buffer->get_pointer(), output_buffer->get_pointer(), output_buffer->num_samples);
    pcm_multiply(output_buffer->get_pointer(), scale_by, output_buffer->num_samples);

    source->notify_buffer(output_buffer);
}

} // namespace audio

} //namespace jackalope
