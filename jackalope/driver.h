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

#include <functional>

#include <jackalope/property.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class domain_t;
class driver_t;

using driver_constructor_t = function_t<shared_t<driver_t> (init_list_t init_list_in)>;

void add_driver_constructor(const string_t& class_name_in, driver_constructor_t constructor_in);
driver_constructor_t get_driver_constructor(const string_t& class_name_in);

class driver_t : public baseobj_t, public shared_obj_t<driver_t>, public lockable_t, protected propobj_t {

protected:
    const init_list_t init_args;
    weak_t<domain_t> domain;

    shared_t<domain_t> get_domain__e();
    virtual void init__e();
    virtual void activate__e();
    virtual void start__e();
    virtual void stop__e();

public:
    static shared_t<driver_t> make(const init_list_t& init_list_in);
    driver_t(const init_list_t& init_list_in);
    virtual void set_domain(shared_t<domain_t>);
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void stop();
};

} // namespace jackalope
