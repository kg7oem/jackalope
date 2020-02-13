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

#include <jackalope/exception.h>

namespace jackalope {

runtime_error_t::runtime_error_t(const std::string& what_in)
: std::runtime_error(what_in)
{ }

runtime_error_t::runtime_error_t(const char * what_in)
: std::runtime_error(what_in)
{ }

} // namespace jackalope
