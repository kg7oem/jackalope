// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#pragma once

#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_OBJECT_LOG_VARGS(level, ...) JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::level_t::level, "(", this->description(), ") ", __VA_ARGS__)
#define object_log_info(...) JACKALOPE_OBJECT_LOG_VARGS(info, __VA_ARGS__)
#define object_log_trace(...) JACKALOPE_OBJECT_LOG_VARGS(trace, __VA_ARGS__)

namespace jackalope {

class object_t : public shared_obj_t<object_t>, public lock_obj_t {

protected:
    bool initialized_flag = false;
    bool activated_flag = false;

    static size_t next_object_id()
    {
        atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);
        return ++current_id;
    }

    virtual void will_init();
    virtual void did_init();
    virtual void will_activate();
    virtual void did_activate();

public:
    const size_t id = next_object_id();

    template <class T, typename... Args>
    static shared_t<T> make(Args... args_in)
    {
        return dynamic_pointer_cast<T>(make(args_in...));
    }

    static shared_t<object_t> make();
    object_t() = default;
    virtual ~object_t() = default;

    virtual void init();
    virtual void activate();
    virtual string_t description();
    // virtual const string_t& get_type();
};

} //namespace jackalope
