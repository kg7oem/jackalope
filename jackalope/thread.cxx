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

void debug_mutex_t::lock()
{
    debug_mutex_t::lock_t lock(mutex);

    if (! is_available__e()) {
        wait__e(lock);
    }

    take__e();
}

bool debug_mutex_t::try_lock()
{
    debug_mutex_t::lock_t lock(mutex);

    if (! is_available__e()) {
        return false;
    }

    take__e();
    return true;
}

void debug_mutex_t::unlock()
{
    debug_mutex_t::lock_t lock(mutex);

    release__e();
}

thread_t::id debug_mutex_t::get_owner_id()
{
    debug_mutex_t::lock_t lock(mutex);

    return owner;
}

bool debug_mutex_t::is_available()
{
    debug_mutex_t::lock_t lock(mutex);
    return is_available__e();
}

bool debug_mutex_t::is_available__e()
{
    return owner == thread_t::id();
}

void debug_mutex_t::take__e()
{
    assert(owner == thread_t::id());
    owner = std::this_thread::get_id();
}

void debug_mutex_t::release__e()
{
    assert(owner == std::this_thread::get_id());

    owner = thread_t::id();
    available_cond.notify_one();
}

void debug_mutex_t::wait__e(lock_t& lock_in)
{
    auto this_thread_id = std::this_thread::get_id();
    assert(owner != this_thread_id);
    assert(waiters.find(this_thread_id) == waiters.end());

    waiters[this_thread_id] = true;
    available_cond.wait(lock_in, [this]{ return is_available__e(); });
    waiters.erase(this_thread_id);
}

lock_t lockable_t::get_object_lock()
{
    return lock_t(object_mutex);
}

} // namespace jackalope
