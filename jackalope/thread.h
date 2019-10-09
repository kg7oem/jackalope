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

#include <condition_variable>
#include <mutex>
#include <thread>

#include <jackalope/types.h>

#define assert_mutex_owner(mutex) assert(mutex.get_owner_id() == std::this_thread::get_id())
#define assert_lock_owner(lock) assert_mutex_owner(*lock.mutex())
#define assert_lockable_owner() assert_mutex_owner(this->object_mutex)

namespace jackalope {

using condition_type = std::condition_variable;
using thread_type = std::thread;

class debug_mutex : public baseobj_t {
public:
    using lock_type = std::unique_lock<std::mutex>;
    using waiters_type = pool_map_t<thread_type::id, bool>;

protected:
    std::mutex mutex;
    condition_type available_cond;
    thread_type::id owner = std::thread::id();
    waiters_type waiters;
    bool is_available__e() noexcept;
    void take__e() noexcept;
    void release__e() noexcept;
    void wait__e(lock_type& lock_in) noexcept;

public:
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;
    bool is_available() noexcept;
    thread_type::id get_owner_id() noexcept;
};

using mutex_type = debug_mutex;
using lock_type = std::unique_lock<mutex_type>;

class lockable {

protected:
    mutex_type object_mutex;
    lock_type get_object_lock();
};

} // namespace jackalope
