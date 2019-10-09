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
#include <jackalope/log/engine.h>

namespace jackalope {

namespace log {

engine * get_engine() noexcept
{
    static engine global_engine;
    return &global_engine;
}

log_event::log_event(const char * source_in, const level_type& level_in, const timestamp& when_in, const thread_type::id& tid_in, const char* function_in, const char *file_in, const int& line_in, const string_type& message_in)
: source(source_in), level(level_in), when(when_in), tid(tid_in), function(function_in), file(file_in), line(line_in), message(message_in)
{ }

bool engine::should_log(const level_type& level_in, const char_type * source_in) noexcept
{
    auto lock = get_object_lock();
    return should_log__e(level_in, source_in);
}

bool engine::should_log__e(const level_type& level_in, const char_type *) noexcept
{
    assert_lockable_owner();

    return level_in >= min_level;
}

void engine::deliver(const log_event& event_in) noexcept
{
    auto lock = get_object_lock();
    deliver__e(event_in);
}

void engine::deliver__e(const log_event& event_in) noexcept
{
    assert_lockable_owner();

    if (! should_log__e(event_in.level, event_in.source)) {
        return;
    }

    for(auto&& i : destinations) {
        i->handle_deliver(event_in);
    }
}

void engine::add_destination(shared_type<dest> dest_in)
{
    auto lock = get_object_lock();
    add_destination__e(dest_in);
}

void engine::add_destination__e(shared_type<dest> dest_in)
{
    assert_lockable_owner();

    if (dest_in->get_min_level() == level_type::uninit) {
        throw runtime_error("dest min_level_type was not initialized");
    }

    destinations.push_back(dest_in);
    update_min_level__e();
}

static level_type find_min_level(const pool_vector_type<shared_type<dest>>& destinations_in) noexcept
{
    if (destinations_in.size() == 0) {
        return level_type::uninit;
    }

    auto min_level = level_type::fatal;

    for(auto&& i : destinations_in) {
        auto&& dest_min_level = i->get_min_level();
        if (dest_min_level < min_level) {
            min_level = dest_min_level;
        }
    }

    return min_level;
}

void engine::update_min_level__e() noexcept
{
    assert_lockable_owner();

    auto new_min_level = find_min_level(destinations);
    min_level = new_min_level;
}

} // namespace log

} // namespace jackalope
