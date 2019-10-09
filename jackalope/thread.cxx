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

#include <cassert>
#include <iostream>

#include <jackalope/logging.h>
#include <jackalope/thread.h>

namespace jackalope {

void debug_mutex::lock() noexcept
{
    debug_mutex::lock_type lock(mutex);

    if (! is_available__e()) {
        wait__e(lock);
    }

    take__e();
}

bool debug_mutex::try_lock() noexcept
{
    debug_mutex::lock_type lock(mutex);

    if (! is_available__e()) {
        return false;
    }

    take__e();
    return true;
}

void debug_mutex::unlock() noexcept
{
    debug_mutex::lock_type lock(mutex);

    release__e();
}

thread_t::id debug_mutex::get_owner_id() noexcept
{
    debug_mutex::lock_type lock(mutex);

    return owner;
}

bool debug_mutex::is_available() noexcept
{
    debug_mutex::lock_type lock(mutex);
    return is_available__e();
}

bool debug_mutex::is_available__e() noexcept
{
    return owner == thread_t::id();
}

void debug_mutex::take__e() noexcept
{
    assert(owner == thread_t::id());
    owner = std::this_thread::get_id();
}

void debug_mutex::release__e() noexcept
{
    assert(owner == std::this_thread::get_id());

    owner = thread_t::id();
    available_cond.notify_one();
}

void debug_mutex::wait__e(lock_type& lock_in) noexcept
{
    auto this_thread_id = std::this_thread::get_id();
    assert(owner != this_thread_id);
    assert(waiters.find(this_thread_id) == waiters.end());

    waiters[this_thread_id] = true;
    available_cond.wait(lock_in, [this]{ return is_available__e(); });
    waiters.erase(this_thread_id);
}

lock_type lockable::get_object_lock()
{
    return lock_type(object_mutex);
}

} // namespace jackalope
