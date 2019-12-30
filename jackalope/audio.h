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

};

class audio_link_t : public link_t {

protected:
    atomic_t<bool> available_flag = ATOMIC_VAR_INIT(false);
    atomic_t<bool> ready_flag = ATOMIC_VAR_INIT(false);

public:
    audio_link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);
    virtual bool is_available() override;
    virtual void set_available(const bool available_in) override;
    virtual bool is_ready() override;
    virtual void set_ready(const bool ready_in) override;
};

class audio_source_t : public source_t {

public:
    audio_source_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual bool _is_available() override;
    virtual shared_t<link_t> make_link(shared_t<source_t> from_in, shared_t<sink_t> to_in) override;
    virtual void link(shared_t<sink_t> sink_in) override;
};

class audio_sink_t : public sink_t {

public:
    audio_sink_t(const string_t name_in, shared_t<object_t> parent_in);
    virtual bool _is_ready();
};

} //namespace jackalope
