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

#include <cstring>

#include <jackalope/log/log_dest.h>

#include "tests.h"

using namespace jackalope;

#define TEST_SOURCE "test source"
#define TEST_LEVEL log::log_level::info
#define TEST_FUNCTION "foo::bar()"
#define TEST_FILE "fakefile.cxx"
#define TEST_LINE 8675309

static const auto test_when = std::chrono::system_clock::now();
static const auto test_tid = thread_type::id();
static const string_type test_message("This is a fine test message");
static const log::log_event test_log_event(TEST_SOURCE, TEST_LEVEL, test_when, test_tid, TEST_FUNCTION, TEST_FILE, TEST_LINE, test_message);

struct test_log_dest : public log::log_dest {
    test_log_dest(const log::log_level min_level_in)
    : log::log_dest(min_level_in)
    { }

    virtual void handle_event__e(const log::log_event& event_in) noexcept
    {
        const log::log_event& test_log_event_ref = test_log_event;
        test_case(&test_log_event_ref == &event_in);
        test_case(std::strcmp(event_in.source, TEST_SOURCE) == 0);
        test_case(event_in.level == TEST_LEVEL);
        test_case(event_in.when == test_when);
        test_case(event_in.tid == test_tid);
        test_case(std::strcmp(event_in.function, TEST_FUNCTION) == 0);
        test_case(std::strcmp(event_in.file, TEST_FILE) == 0);
        test_case(event_in.line == TEST_LINE);
        test_case(event_in.message == test_message);
    }
};

static void log_dest_subclass()
{
    test_log_dest test_dest(log::log_level::trace);

    test_dest.handle_deliver(test_log_event);
}

int main()
{
    start_testing(9);

    run_test(log_dest_subclass);
}
