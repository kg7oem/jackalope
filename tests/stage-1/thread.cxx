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

#include <jackalope/thread.h>

#include "tests.h"

using namespace std::chrono_literals;
using namespace jackalope;

struct test_debug_mutex : public debug_mutex {
    const waiters_type& get_waiters()
    {
        return waiters;
    }
};

static void debug_mutex_lock()
{
    test_debug_mutex test_mutex;

    test_case(test_mutex.get_owner_id() == thread_t::id());
    test_case(test_mutex.get_waiters().size() == 0);
    test_case(test_mutex.is_available());

    test_mutex.lock();
    test_case(test_mutex.get_owner_id() == std::this_thread::get_id());
    test_case(test_mutex.get_waiters().size() == 0);
    test_case(! test_mutex.is_available());

    test_mutex.unlock();
    test_case(test_mutex.get_owner_id() == thread_t::id());
    test_case(test_mutex.get_waiters().size() == 0);
    test_case(test_mutex.is_available());
}

static void debug_mutex_try_lock()
{
    debug_mutex test_mutex;

    test_case(test_mutex.is_available());
    test_case(test_mutex.try_lock());

    test_case(! test_mutex.is_available());
    test_case(! test_mutex.try_lock());

    test_mutex.unlock();
    test_case(test_mutex.try_lock());
    test_case(! test_mutex.try_lock());
}

static void debug_mutex_waiting()
{
    test_debug_mutex test_mutex;
    std::mutex continue_mutex;
    std::unique_lock continue_lock(continue_mutex);
    std::condition_variable continue_condition;
    bool continue_flag = false;

    test_case(test_mutex.is_available());

    test_case(test_mutex.get_waiters().size() == 0);
    test_mutex.lock();
    test_case(test_mutex.get_waiters().size() == 0);
    test_case(! test_mutex.is_available());

    thread_t waiting_thread([&]{
        test_mutex.lock();

        continue_mutex.lock();
        continue_flag = true;
        continue_mutex.unlock();
        continue_condition.notify_all();
    });

    // FIXME this is a poor workaround for the race condition
    // in the thread starting
    std::this_thread::sleep_for(100ms);
    test_case(test_mutex.get_waiters().size() == 1);
    test_case(! test_mutex.is_available());

    test_mutex.unlock();
    continue_condition.wait(continue_lock, [&]{ return continue_flag; });
    test_case(test_mutex.get_waiters().size() == 0);
    test_case(! test_mutex.is_available());

    waiting_thread.join();
}

int main()
{
    start_testing(23);

    run_test(debug_mutex_lock);
    run_test(debug_mutex_try_lock);
    run_test(debug_mutex_waiting);
}
