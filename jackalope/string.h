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
using stringstream_t = std::basic_stringstream<char_t, std::char_traits<char_t>, pool_allocator_t<char_t>>;
using init_args_t = pool_vector_t<std::pair<const string_t, const string_t>>;
using init_list_t = std::initializer_list<std::pair<const string_t, const string_t>>;

template <typename T>
void sstream_accumulate_vaargs(stringstream_t& sstream, T&& t) {
    sstream << t;
}

template <typename T, typename... Args>
void sstream_accumulate_vaargs(stringstream_t& sstream, T&& t, Args&&... args) {
    sstream_accumulate_vaargs(sstream, t);
    sstream_accumulate_vaargs(sstream, args...);
}

template <typename... Args>
string_t to_string(Args&&... args) {
    stringstream_t buf;
    sstream_accumulate_vaargs(buf, args...);
    return buf.str();
}

// from https://stackoverflow.com/a/236803
template<typename Out>
void split_string(const string_t &string_in, const char delim_in, Out result) {
    stringstream_t ss(string_in);
    string_t item;

    while (std::getline(ss, item, delim_in)) {
        *(result++) = item;
    }
}

pool_vector_t<string_t> split_string(const string_t& string_in, const char delim_in);

init_args_t make_init_args(const init_list_t& init_list_in);
init_args_t make_init_args(const pool_map_t<string_t, string_t>& map_in);
bool init_args_has(const char * name_in, const init_args_t& init_args_in);
bool init_args_has(const string_t&, const init_args_t& init_args_in);
string_t init_args_get(const char * name_in, const init_args_t& init_args_in);
string_t init_args_get(const string_t& name_in, const init_args_t& init_args_in);
init_args_t init_args_find(const char * prefix_in, const init_args_t& init_args_in);
init_args_t init_args_find(const string_t& prefix_in, const init_args_t& init_args_in);

}
