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

#include <chrono>

#include <jackalope/log/dest.forward.h>
#include <jackalope/log/engine.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

namespace log {

enum class level_t {
    uninit = -1,
    unknown = 0,
    trace = 10,
    debug = 30,
    verbose = 40,
    info = 50,
    error = 80,
    fatal = 100,
};

struct event_t : public baseobj_t {
    using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

    const char_t * source = nullptr;
    const level_t level = level_t::uninit;
    const timestamp_t when;
    const thread_t::id tid;
    const char *function = nullptr;
    const char *file = nullptr;
    const size_t line = 0;
    const string_t message;

    event_t(const char * source_in, const level_t& level_in, const timestamp_t& when_in, const thread_t::id& tid_in, const char* function_in, const char *file_in, const int& line_in, const string_t& message_in);
    ~event_t() = default;
};

class engine_t : public baseobj_t, public lockable_t {

protected:
    level_t min_level = level_t::uninit;
    pool_vector_t<shared_t<dest_t>> destinations;

    void update_min_level__e();
    bool should_log__e(const level_t& level_in, const char_t * source_in);
    void deliver__e(const event_t& event_in);
    void add_destination__e(shared_t<dest_t> dest_in);

public:
    bool should_log(const level_t& level_in, const char_t * source_in);
    void deliver(const event_t& event_in);
    void add_destination(shared_t<dest_t> dest_in);
};

engine_t * get_engine();

template<typename... Args>
void send_vargs_event(const char * source_in, const level_t& level_in, const char *function_in, const char *path_in, const int& line_in, Args&&... args_in)
{
    if (get_engine()->should_log(level_in, source_in)) {
        auto when = std::chrono::system_clock::now();

        auto tid = std::this_thread::get_id();
        auto message = to_string(args_in...);
        event_t event(source_in, level_in, when, tid, function_in, path_in, line_in, message);

        get_engine()->deliver(event);
    }

    return;
}

} // namespace log

} // namespace jackalope
