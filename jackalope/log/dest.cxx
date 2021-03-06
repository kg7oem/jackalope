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

#include <iostream>

#include <jackalope/log/dest.h>

namespace jackalope {

namespace log {

dest_t::dest_t(const level_t min_level_in)
: min_level(min_level_in)
{ }

level_t dest_t::get_min_level()
{
    auto lock = get_object_lock();
    return get_min_level__e();
}

level_t dest_t::get_min_level__e()
{
    assert_lockable_owner();

    return min_level;
}

void dest_t::handle_deliver(const event_t& event_in)
{
    auto lock = get_object_lock();
    return handle_deliver__e(event_in);
}

void dest_t::handle_deliver__e(const event_t& event_in)
{
    assert_lockable_owner();

    if (event_in.level < min_level) {
        return;
    }

    handle_event__e(event_in);
}

console_dest_t::console_dest_t(const level_t min_level_in)
: dest_t(min_level_in)
{ }

void console_dest_t::handle_event__e(const event_t& event_in)
{
    assert_lockable_owner();

    lock_t console_lock(console_mutex);
    std::cout << event_in.tid << " ";
    // std::cout << event_in.file << ":";
    // std::cout << event_in.line << " ";
    std::cout << event_in.message << std::endl;
}

} // namespace log

} // namespace jackalope
