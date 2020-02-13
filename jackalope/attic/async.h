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

#include <boost/asio.hpp>

#include <jackalope/property.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_ASYNC_PROPERTY_NAME "name"
#define JACKALOPE_ASYNC_PROPERTY_THREADS "threads"

namespace jackalope {

template <typename T>
using async_job_t = function_t<T ()>;

class async_engine_t : public base_t, public shared_obj_t<async_engine_t>, public prop_obj_t {

protected:
    boost::asio::io_context asio_io;
    boost::asio::io_service::work * asio_work = nullptr;
    pool_list_t<thread_t> asio_threads;
    size_t num_threads = 0;

    virtual void init_threads();
    virtual void asio_thread();

public:
    static size_t detect_num_threads();
    static shared_t<async_engine_t> make(const init_args_t& init_args_in);
    async_engine_t(const init_args_t& init_args_in);
    virtual ~async_engine_t();
    void submit_job(async_job_t<void> job_in);
};

void set_async_config(const string_t& value_in);
shared_t<async_engine_t> get_async_engine();

} // namespace jackalope
