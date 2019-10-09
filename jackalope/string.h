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

#include <sstream>

#include <jackalope/types.h>

namespace jackalope {

using string_type = std::basic_string<char_t, std::char_traits<char_t>, allocator_type<char_t>>;
using stringstream_type = std::basic_stringstream<char_t, std::char_traits<char_t>, allocator_type<char_t>>;

template <typename T>
void sstream_accumulate_vaargs(stringstream_type& sstream, T&& t) {
    sstream << t;
}

template <typename T, typename... Args>
void sstream_accumulate_vaargs(stringstream_type& sstream, T&& t, Args&&... args) {
    sstream_accumulate_vaargs(sstream, t);
    sstream_accumulate_vaargs(sstream, args...);
}

template <typename... Args>
string_type vaargs_to_string(Args&&... args) {
    stringstream_type buf;
    sstream_accumulate_vaargs(buf, args...);
    return buf.str();
}

}
