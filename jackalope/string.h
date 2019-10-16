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

using string_t = std::basic_string<char_t, std::char_traits<char_t>, pool_allocator_t<char_t>>;
using stringstring_t = std::basic_stringstream<char_t, std::char_traits<char_t>, pool_allocator_t<char_t>>;

template <typename T>
void sstream_accumulate_vaargs(stringstring_t& sstream, T&& t) {
    sstream << t;
}

template <typename T, typename... Args>
void sstream_accumulate_vaargs(stringstring_t& sstream, T&& t, Args&&... args) {
    sstream_accumulate_vaargs(sstream, t);
    sstream_accumulate_vaargs(sstream, args...);
}

template <typename... Args>
string_t vaargs_to_string(Args&&... args) {
    stringstring_t buf;
    sstream_accumulate_vaargs(buf, args...);
    return buf.str();
}

// from https://stackoverflow.com/a/236803
template<typename Out>
void split_string(const string_t &string_in, const char delim_in, Out result) {
    stringstring_t ss(string_in);
    string_t item;

    while (std::getline(ss, item, delim_in)) {
        *(result++) = item;
    }
}

pool_vector_t<string_t> split_string(const string_t& string_in, const char delim_in);

}
