// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/async.h>
#include <jackalope/exception.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>

#define JACKALOPE_ASYNC_DEFAULT_DOMAIN "default"

namespace jackalope {

static pool_map_t<string_t, string_t> async_config;
static weak_t<async_engine_t> async_engine_cache;

static lock_t get_async_lock()
{
    static mutex_t async_mutex;
    return lock_t(async_mutex);
}

void set_async_config(const string_t& name_in, const string_t& value_in)
{
    auto lock = get_async_lock();

    if (! async_engine_cache.expired()) {
        throw_runtime_error("Can not configure async engine because it is running");
    }

    async_config[name_in] = value_in;
}

shared_t<async_engine_t> get_async_engine()
{
    auto lock = get_async_lock();

    shared_t<async_engine_t> engine = nullptr;

    while(true) {
        try {
            if (async_engine_cache.expired()) {
                auto init_args = make_init_args(async_config);
                async_engine_cache = engine = async_engine_t::make(init_args);
            } else {
                engine = async_engine_cache.lock();
            }

            break;
        } catch (const std::bad_weak_ptr& e) {
            continue;
        }
    }

    assert(engine != nullptr);

    return engine;
}

void async_shutdown()
{
    auto lock = get_async_lock();

    try {
        auto engine = async_engine_cache.lock();

        if (engine != nullptr) {
            log_trace("shutting down async engine");
            engine->shutdown();
        }
    } catch (const std::bad_weak_ptr&) {
        // nothing to do
    }
}

size_t async_engine_t::detect_num_threads()
{
    size_t num_threads = std::thread::hardware_concurrency();

    if (num_threads == 0) {
        num_threads = 1;
    }

    return num_threads;
}

shared_t<async_engine_t> async_engine_t::make(const init_args_t& init_args_in)
{
    return jackalope::make_shared<async_engine_t>(init_args_in);
}

async_engine_t::async_engine_t(const init_args_t&)
{
    asio_work = new boost::asio::io_service::work(asio_io);

    init_threads();
}

async_engine_t::~async_engine_t()
{
    shutdown();
}

void async_engine_t::shutdown()
{
    guard_lockable({ _shutdown(); });
}

void async_engine_t::_shutdown()
{
    assert_lockable_owner();

    if (asio_work != nullptr) {
        delete asio_work;
        asio_work = nullptr;
    }

    _join();
}

void async_engine_t::join()
{
    guard_lockable({
        _join();
    });
}

void async_engine_t::_join()
{
    assert_lockable_owner();

    for(auto& i : asio_threads) {
        i.join();
    }

    asio_threads.clear();
}

void async_engine_t::init_threads()
{
    auto num_threads = detect_num_threads();

    if (num_threads == 0) {
        num_threads = 1;
    }

    for(size_t i = 0; i < num_threads; i++) {
        auto thread = asio_threads.emplace(asio_threads.begin(), std::bind(&async_engine_t::asio_thread, this));
        set_thread_priority(*thread, thread_priority_t::normal);
    }
}

void async_engine_t::asio_thread()
{
    log_info("New ASIO thread has been started");
    asio_io.run();
    log_info("ASIO thread is done running");
}

void async_engine_t::submit_job(async_job_t<void> job_in)
{
    guard_lockable({ _submit_job(job_in); });
}

void async_engine_t::_submit_job(async_job_t<void> job_in)
{
    assert_lockable_owner();

    asio_io.post(job_in);
}

} // namespace jackalope
