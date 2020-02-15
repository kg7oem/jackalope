// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/audio/channel.h>
#include <jackalope/audio/pcm.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>
#include <jackalope/property.h>

namespace jackalope {

audio_buffer_t::audio_buffer_t(const size_t num_samples_in)
: num_samples(num_samples_in)
{
    buffer.reserve(num_samples);

    pcm_zero(get_pointer(), num_samples);
}

audio_buffer_t::~audio_buffer_t()
{ }

real_t * audio_buffer_t::get_pointer()
{
    return buffer.data();
}

audio_link_t::audio_link_t(shared_t<source_t> source_in, shared_t<sink_t> sink_in)
: link_t(source_in, sink_in)
{
    assert(source_in->type == sink_in->type);
}

bool audio_link_t::is_available()
{
    auto lock = get_object_lock();

    return buffer == nullptr;
}

bool audio_link_t::is_ready()
{
    auto lock = get_object_lock();

    return buffer != nullptr;
}

void audio_link_t::reset()
{
    auto source = guard_lockable({
        assert(buffer != nullptr);

        buffer = nullptr;

        return get_from();
    });

    source->link_available(shared_obj());
}

void audio_link_t::set_buffer(shared_t<audio_buffer_t> buffer_in)
{
    auto lock = get_object_lock();

    assert(buffer == nullptr);

    buffer = buffer_in;
}

shared_t<audio_buffer_t> audio_link_t::get_buffer()
{
    auto lock = get_object_lock();

    assert(buffer != nullptr);

    return buffer;
}

shared_t<audio_source_t> audio_source_t::make(const string_t name_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<audio_source_t>(name_in, parent_in);
}

audio_source_t::audio_source_t(const string_t name_in, shared_t<object_t> parent_in)
: source_t(name_in, audio_channel_info_t::type, parent_in)
{ }

// a source is available if none
// of the links are not available
bool audio_source_t::_is_available()
{
    assert_lockable_owner();

    for(auto& i : links) {
        auto available = i->is_available();

        if (! available) {
            return false;
        }
    }

    return true;
}

shared_t <link_t> audio_source_t::make_link(shared_t<source_t> from_in, shared_t<sink_t> to_in)
{
    return jackalope::make_shared<audio_link_t>(from_in, to_in);
}

void audio_source_t::link(shared_t<sink_t> sink_in)
{
    if (sink_in->type != audio_channel_info_t::type) {
        throw_runtime_error("Incompatible types during link: ", type, " -> ", sink_in->type);
    }

    source_t::link(sink_in);
}

void audio_source_t::notify_buffer(shared_t<audio_buffer_t> buffer_in)
{
    auto lock = get_object_lock();
    _notify_buffer(buffer_in);
}

void audio_source_t::_notify_buffer(shared_t<audio_buffer_t> buffer_in)
{
    assert_lockable_owner();

    for(auto i : links) {
        i->shared_obj<audio_link_t>()->set_buffer(buffer_in);
    }

    source_t::_notify();
}

shared_t<audio_sink_t> audio_sink_t::make(const string_t name_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<audio_sink_t>(name_in, parent_in);
}

audio_sink_t::audio_sink_t(const string_t name_in, shared_t<object_t> parent_in)
: sink_t(name_in, audio_channel_info_t::type, parent_in)
{ }

shared_t<audio_buffer_t> audio_sink_t::get_buffer()
{
    auto lock = get_object_lock();

    return _get_buffer();
}

shared_t<audio_buffer_t> audio_sink_t::_get_buffer()
{
    assert_lockable_owner();

    auto buffer_size = get_parent()->get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
    auto links_size = links.size();

    if (links_size == 0) {
        return jackalope::make_shared<audio_buffer_t>(buffer_size);
    } else if (links_size == 1) {
        auto audio_link = links.front()->shared_obj<audio_link_t>();
        return audio_link->get_buffer();
    } else {
        throw_runtime_error("can't handle more than 1 link");
    }
}

// a sink is ready if none
// of the links are not ready
bool audio_sink_t::_is_ready()
{
    assert_lockable_owner();

    for(auto& i : links) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

// a sink is available if none of the
// links are ready
bool audio_sink_t::_is_available()
{
    assert_lockable_owner();

    for(auto& i : links) {
        if (i->is_ready()) {
            return false;
        }
    }

    return true;
}

void audio_sink_t::_reset()
{
    assert_lockable_owner();

    auto parent = get_parent();

    log_info("(", parent->description(), ") reseting sink: ", name);

    for(auto i : links) {
        auto link = i->shared_obj<audio_link_t>();

        if (! link->is_available()) {
            link->reset();
        }
    }
}

void audio_sink_t::_start()
{
    assert_lockable_owner();

    sink_t::_start();
}

void audio_sink_t::_forward(shared_t<source_t> source_in)
{
    assert_lockable_owner();

    auto buffer = _get_buffer();

    source_in->shared_obj<audio_source_t>()->notify_buffer(buffer);
}

const string_t& audio_channel_info_t::get_type() const
{
    return type;
}

const prop_args_t& audio_channel_info_t::get_properties() const
{
    return properties;
}

source_constructor_t audio_channel_info_t::get_source_constructor() const
{
    return source_constructor;
}

sink_constructor_t audio_channel_info_t::get_sink_constructor() const
{
    return sink_constructor;
}

} //namespace jackalope
