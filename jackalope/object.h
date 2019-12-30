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
#include <jackalope/foreign.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class object_t : public shared_obj_t<object_t>, protected lockable_t, public base_t {

    friend foreign::node_t;

protected:
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    bool sources_known_available = false;

public:
    template <class T = object_t, typename... Args>
    static shared_t<T> make(Args... args)
    {
        auto new_object = make_shared<T>(args...);
        auto lock = new_object->get_object_lock();
        new_object->init();
        return new_object;
    }

    virtual shared_t<source_t> add_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const size_t source_num_in);
    virtual void init();
    virtual void start();
    virtual void stop();
    virtual void source_available(shared_t<source_t> available_source_in);
    virtual void slot_source_available(shared_t<source_t> available_source_in);
    virtual void check_sources_available();
    virtual void all_sources_available();
};

} //namespace jackalope
