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
#include <jackalope/property.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class object_t : public prop_obj_t, public lockable_t, public shared_obj_t<object_t> {

protected:
    const init_list_t init_args;
    pool_map_t<string_t, shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<sink_t>> sinks;

public:
    object_t(const init_list_t& init_list_in);
    virtual void init();
    virtual void activate();
    virtual void run();
    virtual void stop();
    virtual shared_t<source_t> add_source(const string_t& name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& name_in);
    virtual shared_t<sink_t> add_sink(const string_t& name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& name_in);
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in);
};

} // namespace jackalope
