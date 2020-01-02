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

#include <cassert>

#include <jackalope/audio/portaudio.h>
#include <jackalope/audio/sndfile.h>
#include <jackalope/async.h>
#include <jackalope/audio.h>

namespace jackalope {

static shared_t<audio_source_t> audio_source_constructor(const string_t& name_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<audio_source_t>(name_in, parent_in);
}

static shared_t<audio_sink_t> audio_sink_constructor(const string_t& name_in, shared_t<object_t> parent_in)
{
    return jackalope::make_shared<audio_sink_t>(name_in, parent_in);
}

void audio_init()
{
    add_source_constructor(JACKALOPE_TYPE_AUDIO, audio_source_constructor);
    add_sink_constructor(JACKALOPE_TYPE_AUDIO, audio_sink_constructor);

    audio::portaudio_init();
    audio::sndfile_init();
}

audio_buffer_t::audio_buffer_t(const size_t num_samples_in)
: num_samples(num_samples_in)
{
    buffer = new real_t[num_samples];

    pcm_zero(buffer, num_samples);
}

audio_buffer_t::~audio_buffer_t()
{
    if (buffer != nullptr) {
        delete buffer;
        buffer = nullptr;
    }
}

real_t * audio_buffer_t::get_pointer()
{
    return buffer;
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

void audio_link_t::reset()
{
    auto lock = get_object_lock();

    buffer = nullptr;
}

bool audio_link_t::is_ready()
{
    auto lock = get_object_lock();

    return buffer != nullptr;
}

shared_t<audio_buffer_t> audio_link_t::get_buffer()
{
    auto lock = get_object_lock();

    assert(buffer != nullptr);

    return buffer;
}

void audio_link_t::set_buffer(shared_t<audio_buffer_t> buffer_in)
{
    auto lock = get_object_lock();

    assert(buffer == nullptr);

    buffer = buffer_in;
}

audio_source_t::audio_source_t(const string_t name_in, shared_t<object_t> parent_in)
: source_t(name_in, JACKALOPE_TYPE_AUDIO, parent_in)
{ }

// a source is available if none
// of the links are not available
bool audio_source_t::_is_available()
{
    assert_lockable_owner();

    for(auto& i : links) {
        if (! i->is_available()) {
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
    if (sink_in->type != JACKALOPE_TYPE_AUDIO) {
        throw_runtime_error("Incompatible types during link: ", type, " -> ", sink_in->type);
    }

    source_t::link(sink_in);
}

void audio_source_t::notify_buffer(shared_t<audio_buffer_t> buffer_in)
{
    auto lock = get_object_lock();

    for (auto i : links) {
        auto link = i->shared_obj<audio_link_t>();

        assert(link->is_available());

        log_info("Setting buffer for sink: ", link->get_to()->name);
        link->set_buffer(buffer_in);

        assert(! link->is_available());

        submit_job([link] {
            auto sink = link->get_to();

            log_info("notifying link ready for sink: ", sink->name);
            sink->link_ready(link);
        });
    }

    _check_available();
}

audio_sink_t::audio_sink_t(const string_t name_in, shared_t<object_t> parent_in)
: sink_t(name_in, JACKALOPE_TYPE_AUDIO, parent_in)
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

void audio_sink_t::_set_links_available()
{
    assert_lockable_owner();

    for(auto link : links) {
        auto source = link->get_from();
        auto audio_link = link->shared_obj<audio_link_t>();

        audio_link->reset();

        submit_job([source, link] {
            source->link_available(link);
        });
    }
}

void audio_sink_t::_reset()
{
    assert_lockable_owner();

    sink_t::_reset();

    known_ready = false;

    _set_links_available();
}

} //namespace jackalope
