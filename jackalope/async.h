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

template <typename T>
using async_job_t = function_t<T ()>;

void async_init();
void async_shutdown();
void submit_job(async_job_t<void> job_in);

template <typename T>
T wait_job(async_job_t<T> job_in)
{
    promise_t<T> promise;

    submit_job([&] {
        auto result = job_in();
        promise.set_value(result);
    });

    return promise.get_future().get();
}

template <>
inline void wait_job(async_job_t<void> job_in)
{
    promise_t<void> promise;

    submit_job([&] {
        job_in();
        promise.set_value();
    });

    promise.get_future().get();
}

} // namespace jackalope
