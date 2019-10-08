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

#include <cstdlib>
#include <iostream>

#include <jackalope/string.h>
#include <jackalope/types.h>

static jackalope::size_type expected_test_cases = 0;
static jackalope::size_type completed_test_cases = 0;

#define run_test(name) { std::cout << #name << " "; name(); }
// FIXME assert() is not the right way to do this
#define test_case(code) { assert(code); completed_test_cases++; }

static void exit_handler()
{
    std::cout << std::endl << "Ran " << completed_test_cases << " test cases" << std::endl;

    if (expected_test_cases != 0 && expected_test_cases != completed_test_cases) {
        std::cerr << "Expected " << expected_test_cases << " test cases but " << completed_test_cases << " were run" << std::endl;
        abort();
    }
}

static void start_testing(const jackalope::size_type expected_test_cases_in)
{
    expected_test_cases = expected_test_cases_in;
    std::atexit(exit_handler);
}
