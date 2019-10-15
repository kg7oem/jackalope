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
#include <jackalope/property.h>
#include <jackalope/string.h>

namespace jackalope {

property_t::property_t(const type_t type_in)
: type(type_in)
{
    switch(type) {
        case type_t::unknown: throw_runtime_error("can not specify unknown as a property type");
        case type_t::size: value.size = 0; break;
        case type_t::integer: value.integer = 0; break;
        case type_t::real: value.real = 0; break;
        case type_t::string: value.string = new string_t; break;
    }
}

property_t::~property_t()
{
    if (type == type_t::string && value.string != nullptr) {
        delete(value.string);
        value.string = nullptr;
    }
}

string_t property_t::get()
{
    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known");
        case type_t::size: return vaargs_to_string(value.size);
        case type_t::integer: return vaargs_to_string(value.integer);
        case type_t::real: return vaargs_to_string(value.real);
        case type_t::string: return *value.string;
    }

    throw_runtime_error("should never get out of switch statement");
}

void property_t::set(const double value_in)
{
    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known"); return;
        case type_t::size: set_size(value_in); return;
        case type_t::integer: set_integer(value_in); return;
        case type_t::real: set_real(value_in); return;
        case type_t::string: set_string(vaargs_to_string(value_in));
    }

    throw_runtime_error("should never get out of switch statement");
}

void property_t::set(const string_t& value_in)
{
    auto c_str = value_in.c_str();

    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known");
        case type_t::size: value.size = std::strtoul(c_str, nullptr, 0); return;
        case type_t::integer: value.integer = std::atoi(c_str); return;
        case type_t::real: value.real = std::strtof(c_str, nullptr); return;
        case type_t::string: *value.string = value_in; return;
    }

    throw_runtime_error("should never get out of switch statement");
}

size_t& property_t::get_size()
{
    if (type != type_t::size) {
        throw_runtime_error("property is not of type: size");
    }

    return value.size;
}

void property_t::set_size(const size_t size_in)
{
    if (type != type_t::size) {
        throw_runtime_error("property is not of type: size");
    }

    value.size = size_in;
}

int_t& property_t::get_integer()
{
    if (type != type_t::integer) {
        throw_runtime_error("property is not of type: integer");
    }

    return value.integer;
}

void property_t::set_integer(const int_t integer_in)
{
    if (type != type_t::integer) {
        throw_runtime_error("property is not of type: integer");
    }

    value.integer = integer_in;
}

void property_t::set_real(const real_t real_in)
{
    if (type != type_t::real) {
        throw_runtime_error("property is not of type: real");
    }

    value.real = real_in;
}

real_t& property_t::get_real()
{
    if (type != type_t::real) {
        throw_runtime_error("property is not of type: real");
    }

    return value.real;
}

void property_t::set_string(const string_t& string_in)
{
    if (type != type_t::string) {
        throw_runtime_error("property is not of type: string");
    }

    *value.string = string_in;
}

string_t& property_t::get_string()
{
    if (type != type_t::string) {
        throw_runtime_error("property is not of type: string");
    }

    return *value.string;
}

} // namespace jackalope
