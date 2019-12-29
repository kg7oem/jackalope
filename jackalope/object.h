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

#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class object_t : public shared_obj_t<object_t>, public lockable_t, public base_t {

public:
    template <typename... Args>
    static shared_t<object_t> make(Args... args)
    {
        auto new_object = make_shared<object_t>(args...);
        auto lock = new_object->get_object_lock();
        new_object->init();
        return new_object;
    }

    virtual void init();
    virtual void start();
    virtual void stop();
};

} //namespace jackalope
