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
#include <cstring>
#include <iostream>
#include <pthread.h>

#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/thread.h>

namespace jackalope {

void _assert_mutex_owner(mutex_t& mutex_in, const char * file_in, const size_t line_in)
{
    if (mutex_in.get_owner_id() != std::this_thread::get_id()) {
        jackalope_panic(file_in, ":", line_in, ": thread did not own mutex");
    }
}

// from https://stackoverflow.com/a/31652324
void set_thread_priority(thread_t& thread_in, const thread_priority_t priority_in)
{
    sched_param sch_params;
    sch_params.sched_priority = static_cast<int>(priority_in);

    if (auto error = pthread_setschedparam(thread_in.native_handle(), SCHED_RR, &sch_params)) {
        log_info("could not set thread to realtime priority: ", strerror(error));
    }
}

void debug_mutex_t::lock() noexcept
{
    debug_mutex_t::lock_t lock(mutex);

    if (! is_available__e()) {
        wait__e(lock);
    }

    take__e();
}

bool debug_mutex_t::try_lock() noexcept
{
    debug_mutex_t::lock_t lock(mutex);

    if (! is_available__e()) {
        return false;
    }

    take__e();
    return true;
}

void debug_mutex_t::unlock() noexcept
{
    debug_mutex_t::lock_t lock(mutex);

    release__e();
}

thread_t::id debug_mutex_t::get_owner_id() noexcept
{
    debug_mutex_t::lock_t lock(mutex);

    return owner;
}

bool debug_mutex_t::is_available() noexcept
{
    debug_mutex_t::lock_t lock(mutex);
    return is_available__e();
}

bool debug_mutex_t::is_available__e() noexcept
{
    return owner == thread_t::id();
}

void debug_mutex_t::take__e() noexcept
{
    assert(owner == thread_t::id());
    owner = std::this_thread::get_id();
}

void debug_mutex_t::release__e() noexcept
{
    assert(owner == std::this_thread::get_id());

    owner = thread_t::id();
    available_cond.notify_one();
}

void debug_mutex_t::wait__e(lock_t& lock_in) noexcept
{
    auto this_thread_id = std::this_thread::get_id();
    assert(owner != this_thread_id);
    assert(waiters.find(this_thread_id) == waiters.end());

    waiters[this_thread_id] = true;
    available_cond.wait(lock_in, [this]{ return is_available__e(); });
    waiters.erase(this_thread_id);
}

bool lockable_t::thread_owns_mutex() noexcept
{
    return object_mutex.get_owner_id() == std::this_thread::get_id();
}

lock_t lockable_t::get_object_lock() noexcept
{
    return lock_t(object_mutex);
}

} // namespace jackalope
