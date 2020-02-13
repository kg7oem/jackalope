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
#include <future>
#include <mutex>
#include <thread>

#include <jackalope/types.h>

#define assert_mutex_owner(mutex) assert(mutex.get_owner_id() == std::this_thread::get_id())
#define assert_lock_owner(lock) assert_mutex_owner(*lock.mutex())
#define assert_object_owner(object) assert(object->thread_owns_mutex())
#define assert_lockable_owner() assert_object_owner(this)

#define guard_object(object, block) [&] { auto ___guard_object_lock = object->get_object_lock(); return [&object] block(); }()
#define guard_lockable(block) guard_object(this, block)

namespace jackalope {

class lockable_t;

using condition_t = std::condition_variable_any;
template <typename T>
using future_t = std::future<T>;
template <typename T>
using promise_t = std::promise<T>;
using thread_t = std::thread;

enum class thread_priority_t : int {
    lowest = 1,
    normal = 5,
    highest = 10,
};

void set_thread_priority(thread_t& thread_in, const thread_priority_t priority_in);

class debug_mutex_t : public base_obj_t {
public:
    using lock_t = std::unique_lock<std::mutex>;
    using waiters_t = pool_map_t<thread_t::id, bool>;

protected:
    std::mutex mutex;
    condition_t available_cond;
    thread_t::id owner = std::thread::id();
    waiters_t waiters;
    bool _is_available() noexcept;
    void _take() noexcept;
    void _release() noexcept;
    void _wait(lock_t& lock_in) noexcept;

public:
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;
    bool is_available() noexcept;
    thread_t::id get_owner_id() noexcept;
};

using mutex_t = debug_mutex_t;
using lock_t = std::unique_lock<mutex_t>;

class lock_obj_t {

protected:
    mutex_t object_mutex;

public:
    bool thread_owns_mutex() noexcept;
    lock_t get_object_lock() noexcept;
};

} // namespace jackalope
