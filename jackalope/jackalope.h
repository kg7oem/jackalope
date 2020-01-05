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

#include <iostream>

#include <jackalope/foreign.h>

#define jackalope_panic(...) { auto message = jackalope::to_string(__VA_ARGS__); std::cerr << message << std::endl; abort(); }

namespace jackalope {

void init();
void shutdown();

} // namespace jackalope
