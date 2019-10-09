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

#include <jackalope/log/dest.forward.h>
#include <jackalope/log/engine.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

namespace log {

class dest : public baseobj_t, public lockable_t {

protected:
    level_t min_level = level_t::uninit;

    level_t get_min_level__e() noexcept;
    virtual void handle_event__e(const event_t& event_in) noexcept = 0;
    virtual void handle_deliver__e(const event_t& event_in) noexcept;

public:
    dest(const level_t min_level_in);
    level_t get_min_level() noexcept;
    virtual void handle_deliver(const event_t& event_in) noexcept;
};

class console_dest : public dest {

protected:
    mutex_t console_mutex;

public:
    console_dest(const level_t min_level_in);
    void handle_event__e(const event_t& event_in) noexcept;
};

} // namespace log

} // namespace jackalope
