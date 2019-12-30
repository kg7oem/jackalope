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

#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

template <typename... Args>
class signal_t : public base_t {

public:
    using slot_t = function_t<void (Args...)>;
    using wait_promise_t = std::promise<void>;
    using wait_future_t = std::future<void>;

protected:
    pool_vector_t<slot_t> subscriptions;
    pool_vector_t<promise_t<void>> waiters;
    const weak_t<object_t> parent;

public:
    // thread safe because parent is const
    shared_t<object_t> get_parent()
    {
        return parent.lock();
    }

    void subscribe(slot_t slot_in)
    {
        assert_object_owner(get_parent());

        subscriptions.push_back(slot_in);
    }

    void wait()
    {
        wait_future_t future;

        {
            auto lock = get_parent()->get_object_lock();
            future = waiters.emplace_back().get_future();
        }

        future.get();
    }
};

} //namespace jackalope
