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

#include <jackalope/driver.h>
#include <jackalope/node.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class domain_t;

template <typename... Args>
shared_t<domain_t> make_domain(Args... args)
{
    auto domain = make_shared<domain_t>(args...);
    domain->init();
    return domain;
}

class domain_t : public baseobj_t, public shared_obj_t<domain_t>, protected lockable_t {

protected:
    virtual void init__e();
    virtual void run__e();

public:
    virtual ~domain_t() = default;
    virtual void init();
    virtual void run();
    virtual shared_t<driver_t> add_driver(const init_list_t& init_list_in);
    virtual shared_t<node_t> add_node(const init_list_t& init_list_in);
};

} // namespace jackalope
