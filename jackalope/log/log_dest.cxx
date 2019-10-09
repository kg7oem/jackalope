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

#include <jackalope/log/log_dest.h>

namespace jackalope {

namespace log {

log_dest::log_dest(const log_level min_level_in)
: min_level(min_level_in)
{ }

log_level log_dest::get_min_level() noexcept
{
    auto lock = get_object_lock();
    return get_min_level__e();
}

log_level log_dest::get_min_level__e() noexcept
{
    assert_lockable_owner();

    return min_level;
}

void log_dest::handle_deliver(const log_event& event_in) noexcept
{
    auto lock = get_object_lock();
    return handle_deliver__e(event_in);
}

void log_dest::handle_deliver__e(const log_event& event_in) noexcept
{
    assert_lockable_owner();

    if (event_in.level < min_level) {
        return;
    }

    handle_event__e(event_in);
}

log_console::log_console(const log_level min_level_in)
: log_dest(min_level_in)
{ }

void log_console::handle_event__e(const log_event& event_in) noexcept
{
    assert_lockable_owner();

    lock_type console_lock(console_mutex);
    std::cout << event_in.message << std::endl;
}

} // namespace log

} // namespace jackalope
