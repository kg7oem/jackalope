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

#include <jackalope/property.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class object_t : public prop_obj_t, public lockable_t {

protected:
    const init_list_t init_args;

public:
    object_t(const init_list_t& init_list_in);
    virtual void init();
    virtual void init__e();
    virtual void activate();
    virtual void activate__e();
    virtual void run();
    virtual void run__e();
    virtual void stop();
    virtual void stop__e();
};

} // namespace jackalope
