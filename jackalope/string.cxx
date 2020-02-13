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
#include <jackalope/logging.h>
#include <jackalope/string.h>

namespace jackalope {

// from https://stackoverflow.com/a/236803
pool_vector_t<string_t> split_string(const string_t& string_in, const char delim_in)
{
    pool_vector_t<string_t> parts;

    split_string(string_in, delim_in, std::back_inserter(parts));

    return parts;
}

init_args_t make_init_args(const init_list_t& init_list_in)
{
    init_args_t init_args;

    for(auto&& i : init_list_in) {
        init_args.emplace_back(i);
    }

    return init_args;
}

init_args_t make_init_args(const pool_map_t<string_t, string_t>& map_in) {
    init_args_t init_args;

    for(auto i : map_in) {
        init_args.push_back({ i.first, i.second });
    }

    return init_args;
}

bool init_args_has(const char * name_in, const init_args_t * init_args_in)
{
    const auto name = to_string(name_in);

    for(auto&& i : *init_args_in) {
        if (name == i.first) {
            return true;
        }
    }

    return false;
}

string_t init_args_get(const char * name_in, const init_args_t * init_args_in)
{
    const auto name = to_string(name_in);

    for(auto&& i : *init_args_in) {
        if (name == i.first) {
            return i.second;
        }
    }

    jackalope_throw_runtime_error("could not find init arg: ", name_in);
}

init_args_t init_args_find(const char * prefix_in, const init_args_t * init_args_in)
{
    auto prefix = to_string(prefix_in);
    init_args_t found;

    for (auto& i : *init_args_in) {
        auto parts = split_string(i.first, '.');

        if (parts.front() == prefix) {
            found.push_back(i);
        }
    }

    return found;
}

} // namespace jackalope
