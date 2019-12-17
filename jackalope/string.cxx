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

#include <jackalope/exception.h>
#include <jackalope/string.h>

namespace jackalope {

// from https://stackoverflow.com/a/236803
pool_vector_t<string_t> split_string(const string_t& string_in, const char delim_in)
{
    pool_vector_t<string_t> parts;

    split_string(string_in, delim_in, std::back_inserter(parts));

    return parts;
}

bool init_list_has(const char * name_in, const init_list_t& init_list_in)
{
    auto name = to_string(name_in);

    for(auto i : init_list_in) {
        if (i.first == name) {
            return true;
        }
    }

    return false;
}

string_t init_list_get(const char * name_in, const init_list_t& init_list_in)
{
    auto name = to_string(name_in);

    for(auto i : init_list_in) {
        if (i.first == name) {
            return i.second;
        }
    }

    throw_runtime_error("could not find init arg: ", name);
}

pool_list_t<std::pair<const string_t, const string_t>> init_list_find(const char * prefix_in, const init_list_t& init_list_in)
{
    auto prefix = to_string(prefix_in);
    pool_list_t<std::pair<const string_t, const string_t>> found;

    for (auto& i : init_list_in) {
        auto parts = split_string(i.first, ':');

        if (parts.front() == prefix) {
            found.push_back(i);
        }
    }

    return found;
}

} // namespace jackalope
