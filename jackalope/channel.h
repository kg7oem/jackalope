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

#include <jackalope/channel.forward.h>
#include <jackalope/types.h>

namespace jackalope {

size_t get_link_id();

struct link_t : public base_t, public shared_obj_t<link_t> {

protected:
    const weak_t<source_t> from;
    const weak_t<sink_t> to;

public:
    const size_t id = get_link_id();

    link_t(shared_t<source_t> from_in, shared_t<sink_t> to_in);
    shared_t<source_t> get_from();
    shared_t<sink_t> get_to();
};

struct channel_t : public base_t {

protected:
    channel_t() = default;
    virtual ~channel_t() = default;
};

struct source_t : public channel_t, public shared_obj_t<source_t> {

};

struct sink_t : public channel_t, public shared_obj_t<sink_t> {

};

} //namespace jackalope
