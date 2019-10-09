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

#include <jackalope/log/log_engine.h>

#define JACKALOPE_LOG_NAME "jackalope"

#define JACKALOPE_LOG_VARGS(logname, log_level, ...) jackalope::logging::send_vargs_log_event(logname, log_level, __PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
// #define JACKALOPE_LOG_LAMBDA(logname, log_level, block) jackalope::send_lambda_log_event(logname, log_level, __PRETTY_FUNCTION__, __FILE__, __LINE__, [&]() -> jackalope::string_type block)

#define log_info(...)     JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log_level::info, __VA_ARGS__)

namespace jackalope {

namespace logging {

log_engine * get_engine() noexcept;

template<typename... Args>
void send_vargs_log_event(const char * source_in, const log_level& level_in, const char *function_in, const char *path_in, const int& line_in, Args&&... args_in) noexcept
{
    if (get_engine()->should_log(level_in, source_in)) {
        auto when = std::chrono::system_clock::now();

        auto tid = std::this_thread::get_id();
        auto message = vaargs_to_string(args_in...);
        log_event event(source_in, level_in, when, tid, function_in, path_in, line_in, message);

        get_engine()->deliver(event);
    }

    return;
}

} // namespace logging

} // namespace jackalope
