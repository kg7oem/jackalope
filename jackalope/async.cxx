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

async_engine_t::async_engine_t(const init_args_t& init_args_in)
{
    add_property(JACKALOPE_ASYNC_PROPERTY_THREADS, property_t::type_t::size);

    for(auto i : init_args_in) {
        auto property = get_property(i.first);
        property->set(i.second);
    }

    auto threads_property = get_property(JACKALOPE_ASYNC_PROPERTY_THREADS);
    if (! threads_property->is_defined()) {
        threads_property->set(detect_num_threads());
    }

    asio_work = new boost::asio::io_service::work(asio_io);

    init_threads();
}

async_engine_t::~async_engine_t()
{
    if (asio_work != nullptr) {
        delete asio_work;
        asio_work = nullptr;
    }

    for(auto& i : asio_threads) {
        i.join();
    }

    asio_threads.clear();
}

void async_engine_t::init_threads()
{
    auto num_threads = get_property(JACKALOPE_ASYNC_PROPERTY_THREADS)->get_size();

    if (num_threads == 0) {
        throw_runtime_error("Number of threads must be non-zero");
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
    asio_io.post(job_in);
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

} // namespace jackalope
