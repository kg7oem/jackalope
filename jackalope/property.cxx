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

bool property_t::is_defined()
{
    return defined_flag;
}

string_t property_t::get()
{
    if (! defined_flag) {
        return "(undefined)";
    }

    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known");
        case type_t::size: return to_string(value.size);
        case type_t::integer: return to_string(value.integer);
        case type_t::real: return to_string(value.real);
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
        case type_t::string: set_string(to_string(value_in));
    }

    throw_runtime_error("should never get out of switch statement");
}

void property_t::set(const string_t& value_in)
{
    auto c_str = value_in.c_str();

    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known");
        case type_t::size: set_size(std::strtoul(c_str, nullptr, 0)); return;
        case type_t::integer: set_integer(std::atoi(c_str)); return;
        case type_t::real: set_real(std::strtof(c_str, nullptr)); return;
        case type_t::string: set_string(value_in); return;
    }

    throw_runtime_error("should never get out of switch statement");
}

size_t& property_t::get_size()
{
    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
    }

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

    defined_flag = true;
    value.size = size_in;
}

int_t& property_t::get_integer()
{
    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
    }

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

    defined_flag = true;
    value.integer = integer_in;
}

void property_t::set_real(const real_t real_in)
{
    if (type != type_t::real) {
        throw_runtime_error("property is not of type: real");
    }

    defined_flag = true;
    value.real = real_in;
}

real_t& property_t::get_real()
{
    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
    }

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

    defined_flag = true;
    *value.string = string_in;
}

string_t& property_t::get_string()
{
    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
    }

    if (type != type_t::string) {
        throw_runtime_error("property is not of type: string");
    }

    return *value.string;
}

property_t& prop_obj_t::add_property(const string_t& name_in, property_t::type_t type_in)
{
    auto result = properties.emplace(std::make_pair(name_in, type_in));

    if (! result.second) {
        throw_runtime_error("Attempt to add duplicate property name: ", name_in);
    }

    auto& property = result.first->second;

    return property;
}

property_t& prop_obj_t::get_property(const string_t& name_in)
{
    auto found = properties.find(name_in);

    if (found == properties.end()) {
        throw_runtime_error("Could not find property: ", name_in);
    }

    return found->second;
}


} // namespace jackalope
