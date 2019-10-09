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

enum class log_level {
    uninit = -1,
    unknown = 0,
    trace = 10,
    debug = 30,
    verbose = 40,
    info = 50,
    error = 80,
    fatal = 100,
};

struct log_event : public baseobj {
    using timestamp = std::chrono::time_point<std::chrono::system_clock>;

    const char_type * source = nullptr;
    const log_level level = log_level::uninit;
    const timestamp when;
    const thread_type::id tid;
    const char *function = nullptr;
    const char *file = nullptr;
    const size_type line = 0;
    const string_type message;

    log_event(const char * source_in, const log_level& level_in, const timestamp& when_in, const thread_type::id& tid_in, const char* function_in, const char *file_in, const int& line_in, const string_type& message_in);
    ~log_event() = default;
};

class engine : public baseobj, public lockable {

protected:
    log_level min_level = log_level::uninit;
    pool_vector_type<shared_type<dest>> destinations;

    void update_min_level__e() noexcept;
    bool should_log__e(const log_level& level_in, const char_type * source_in) noexcept;
    void deliver__e(const log_event& event_in) noexcept;
    void add_destination__e(shared_type<dest> dest_in);

public:
    bool should_log(const log_level& level_in, const char_type * source_in) noexcept;
    void deliver(const log_event& event_in) noexcept;
    void add_destination(shared_type<dest> dest_in);
};

} // namespace log

} // namespace jackalope
