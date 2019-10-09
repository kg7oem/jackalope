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

#include <jackalope/log/engine.h>

#define JACKALOPE_LOG_NAME "jackalope"

#define JACKALOPE_LOG_VARGS(logname, log_level, ...) jackalope::log::send_vargs_log_event(logname, log_level, __PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
// #define JACKALOPE_LOG_LAMBDA(logname, log_level, block) jackalope::send_lambda_log_event(logname, log_level, __PRETTY_FUNCTION__, __FILE__, __LINE__, [&]() -> jackalope::string_type block)

#define log_info(...)     JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::log_level::info, __VA_ARGS__)
