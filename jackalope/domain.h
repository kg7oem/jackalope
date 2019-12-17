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
#include <jackalope/driver.h>
#include <jackalope/node.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class domain_t;

class domain_t : public baseobj_t, public shared_obj_t<domain_t>, public propobj_t, protected lockable_t {

protected:
    const init_list_t init_args;
    pool_list_t<shared_t<driver_t>> drivers;
    pool_map_t<string_t, shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<sink_t>> sinks;

    virtual void init__e();
    virtual void run__e();
    virtual void stop__e();

public:
    static shared_t<domain_t> make(const init_list_t& init_list_in);
    domain_t(const init_list_t& init_list_in);
    virtual ~domain_t() = default;
    virtual void init();
    virtual void run();
    virtual void stop();
    virtual shared_t<source_t> add_source(const string_t& name_in, const string_t& type_in);
    virtual shared_t<sink_t> add_sink(const string_t& name_in, const string_t& type_in);
    virtual shared_t<driver_t> add_driver(const init_list_t& init_list_in);
    virtual shared_t<driver_t> add_driver__e(const init_list_t& init_list_in);
    virtual shared_t<node_t> add_node(const init_list_t& init_list_in);
};

} // namespace jackalope
