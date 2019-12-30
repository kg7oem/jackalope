// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <boost/asio.hpp>

#include <jackalope/async.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>

namespace jackalope {

boost::asio::io_service::work * asio_work = nullptr;
pool_list_t<thread_t> asio_threads;

static lock_t get_async_lock()
{
    static mutex_t async_mutex;
    return lock_t(async_mutex);
}

static boost::asio::io_context * get_asio_io()
{
    static boost::asio::io_context boost_io;
    return &boost_io;
}

static void async_thread()
{
    log_info("New ASIO thread has been started");
    get_asio_io()->run();
}

void async_init()
{
    size_t num_threads = std::thread::hardware_concurrency();

    if (num_threads == 0) {
        num_threads = 1;
    }

    auto lock = get_async_lock();

    if (asio_work != nullptr) {
        throw_runtime_error("async_init() called more than once");
    }

    asio_work = new boost::asio::io_service::work(*get_asio_io());

    for(size_t i = 0; i < num_threads; i++) {
        asio_threads.emplace(asio_threads.begin(), async_thread);
    }
}

void async_shutdown()
{
    auto lock = get_async_lock();

    delete asio_work;
    asio_work = nullptr;

    for(auto& i : asio_threads) {
        i.join();
    }

    asio_threads.empty();
}

void submit_job(async_job_t<void> job_in)
{
    get_asio_io()->post(job_in);
}

} // namespace jackalope
