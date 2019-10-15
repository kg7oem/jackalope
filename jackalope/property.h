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

namespace jackalope {

class property_t : public baseobj_t {

public:
    enum class type_t { unknown, size, integer, real, string };

protected:
    union container_t {
        size_t size;
        int_t integer;
        real_t real;
        string_t * string;
    };

    container_t value;

public:
    const type_t type = type_t::unknown;

    property_t(const type_t type_in);
    property_t(const type_t type_in, const double value_in);
    property_t(const type_t type_in, const string_t& value_in);
    virtual ~property_t();
    string_t get();
    void set(const double value_in);
    void set(const string_t& value_in);
    size_t& get_size();
    void set_size(const size_t size_in);
    int_t& get_integer();
    void set_integer(const int_t integer_in);
    void set_real(const real_t real_in);
    real_t& get_real();
    string_t& get_string();
    void set_string(const string_t& string_in);
};

} // namespace jackalope
