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

#include <jackalope/string.h>
#include <jackalope/types.h>

#define throw_vargs(exception_type, ...) { throw exception_type(jackalope::to_string(__VA_ARGS__).c_str()); }
#define throw_runtime_error(...) throw_vargs(jackalope::runtime_error_t, jackalope::to_string(__FILE__, ":", __LINE__, " ", __VA_ARGS__))
