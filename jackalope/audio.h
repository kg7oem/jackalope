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

#pragma once

#include <jackalope/channel.h>
#include <jackalope/types.h>

#define JACKALOPE_TYPE_AUDIO "audio"

namespace jackalope {

void audio_init();

class audio_buffer_t : public base_t, shared_obj_t<audio_buffer_t> {

protected:
    pool_vector_t<real_t> buffer;

public:
    const size_t num_samples;

    audio_buffer_t(const size_t num_samples_in);
    virtual ~audio_buffer_t();
    real_t * get_pointer();
};

class audio_link_t : public link_t, lockable_t {

protected:
    shared_t<audio_buffer_t> buffer = nullptr;

public:
    audio_link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);
    virtual void reset();
    virtual bool is_available() override;
    virtual bool is_ready() override;
    virtual shared_t<audio_buffer_t> get_buffer();
    virtual void set_buffer(shared_t<audio_buffer_t> buffer_in);
};

class audio_source_t : public source_t {

public:
    audio_source_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual bool _is_available() override;
    virtual shared_t<link_t> make_link(shared_t<source_t> from_in, shared_t<sink_t> to_in) override;
    virtual void link(shared_t<sink_t> sink_in) override;
    virtual void notify(shared_t<audio_buffer_t> buffer_in);
    virtual void _notify(shared_t<audio_buffer_t> buffer_in);
};

class audio_sink_t : public sink_t {

public:
    audio_sink_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual shared_t<audio_buffer_t> get_buffer();
    virtual shared_t<audio_buffer_t> _get_buffer();
    virtual bool _is_available() override;
    virtual bool _is_ready() override;
    virtual void _start() override;
    virtual void _reset() override;
    virtual void _forward(shared_t<source_t> source_in) override;
};

} //namespace jackalope
