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
#include <jackalope/logging.h>
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
    auto lock = get_object_lock();

    return defined_flag;
}

string_t property_t::get()
{
    auto lock = get_object_lock();

    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
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

// thread safe because it only calls safe methods
void property_t::set(const double value_in)
{
    switch(type) {
        case type_t::unknown: throw_runtime_error("property type was not known"); return;
        case type_t::size: set_size(value_in); return;
        case type_t::integer: set_integer(value_in); return;
        case type_t::real: set_real(value_in); return;
        case type_t::string: set_string(to_string(value_in)); return;
    }

    throw_runtime_error("should never get out of switch statement");
}

// thread safe because it only calls safe methods
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
    auto lock = get_object_lock();

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
    auto lock = get_object_lock();

    if (type != type_t::size) {
        throw_runtime_error("property is not of type: size");
    }

    defined_flag = true;
    value.size = size_in;
}

int_t& property_t::get_integer()
{
    auto lock = get_object_lock();

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
    auto lock = get_object_lock();

    if (type != type_t::integer) {
        throw_runtime_error("property is not of type: integer");
    }

    defined_flag = true;
    value.integer = integer_in;
}

void property_t::set_real(const real_t real_in)
{
    auto lock = get_object_lock();

    if (type != type_t::real) {
        throw_runtime_error("property is not of type: real");
    }

    defined_flag = true;
    value.real = real_in;
}

real_t& property_t::get_real()
{
    auto lock = get_object_lock();

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
    auto lock = get_object_lock();

    if (type != type_t::string) {
        throw_runtime_error("property is not of type: string");
    }

    defined_flag = true;
    *value.string = string_in;
}

// only safe to return a copy of a string
// because the property container is never
// locked
string_t property_t::get_string()
{
    auto lock = get_object_lock();

    if (! defined_flag) {
        throw_runtime_error("Use of undefined property");
    }

    if (type != type_t::string) {
        throw_runtime_error("property is not of type: string");
    }

    return *value.string;
}

lock_t prop_obj_t::get_property_lock()
{
    return lock_t(property_mutex);
}

shared_t<property_t> prop_obj_t::_add_property(const string_t& name_in, const property_t::type_t type_in)
{
    assert_mutex_owner(property_mutex);

    if (_has_property(name_in)) {
        throw_runtime_error("attempt to add duplicate property name: ", name_in);
    }

    return properties[name_in] = property_t::make(type_in);
}

shared_t<property_t> prop_obj_t::_add_property(const string_t& name_in, const property_t::type_t type_in, const double value_in)
{
    assert_mutex_owner(property_mutex);

    auto property =_add_property(name_in, type_in);
    property->set(value_in);

    return property;
}

shared_t<property_t> prop_obj_t::_add_property(const string_t& name_in, const property_t::type_t type_in, const string_t& value_in)
{
    assert_mutex_owner(property_mutex);

    auto property = _add_property(name_in, type_in);
    property->set(value_in);

    return property;
}

void prop_obj_t::_add_properties(const prop_args_t& properties_in)
{
    assert_mutex_owner(property_mutex);

    for(auto&& i : properties_in) {
        _add_property(i.first, i.second);
    }
}

shared_t<property_t> prop_obj_t::add_property(const string_t& name_in, const property_t::type_t type_in)
{
    auto lock = get_property_lock();
    return _add_property(name_in, type_in);
}

shared_t<property_t> prop_obj_t::add_property(const string_t& name_in, const property_t::type_t type_in, const double value_in)
{
    auto lock = get_property_lock();
    return _add_property(name_in, type_in, value_in);
}

shared_t<property_t> prop_obj_t::add_property(const string_t& name_in, const property_t::type_t type_in, const string_t& value_in)
{
    auto lock = get_property_lock();
    return _add_property(name_in, type_in, value_in);
}

void prop_obj_t::add_properties(const prop_args_t& properties_in)
{
    auto lock = get_property_lock();
    return _add_properties(properties_in);
}

bool prop_obj_t::has_property(const string_t& name_in)
{
    auto lock = get_property_lock();

    return _has_property(name_in);
}

bool prop_obj_t::_has_property(const string_t& name_in)
{
    assert_mutex_owner(property_mutex);

    return properties.count(name_in) != 0;
}

shared_t<property_t> prop_obj_t::get_property(const string_t& name_in)
{
    auto lock = get_property_lock();
    return _get_property(name_in);
}

shared_t<property_t> prop_obj_t::_get_property(const string_t& name_in)
{
    assert_mutex_owner(property_mutex);

    auto found = properties.find(name_in);

    if (found == properties.end()) {
        throw_runtime_error("Could not find property: ", name_in);
    }

    return found->second;
}

pool_vector_t<string_t> prop_obj_t::get_property_names()
{
    auto lock = get_property_lock();

    pool_vector_t<string_t> names;

    for(auto i : properties) {
        names.push_back(i.first);
    }

    return names;
}

void prop_obj_t::set_property(const string_t& name_in, const double value_in)
{
    auto lock = get_property_lock();
    _set_property(name_in, value_in);
}

void prop_obj_t::_set_property(const string_t& name_in, const double value_in)
{
    assert_mutex_owner(property_mutex);
    _get_property(name_in)->set(value_in);
}

void prop_obj_t::set_property(const string_t& name_in, const string_t& value_in)
{
    auto lock = get_property_lock();
    _set_property(name_in, value_in);
}

void prop_obj_t::_set_property(const string_t& name_in, const string_t& value_in)
{
    assert_mutex_owner(property_mutex);
    _get_property(name_in)->set(value_in);
}

void prop_obj_t::set_undef_property(const string_t& name_in, const double value_in)
{
    auto lock = get_property_lock();
    _set_undef_property(name_in, value_in);
}

void prop_obj_t::_set_undef_property(const string_t& name_in, const double value_in)
{
    assert_mutex_owner(property_mutex);

    auto property = _get_property(name_in);

    if (! property->is_defined()) {
        property->set(value_in);
    }
}

void prop_obj_t::set_undef_property(const string_t& name_in, const string_t& value_in)
{
    auto lock = get_property_lock();
    _set_undef_property(name_in, value_in);
}

void prop_obj_t::_set_undef_property(const string_t& name_in, const string_t& value_in)
{
    assert_mutex_owner(property_mutex);

    auto property = _get_property(name_in);

    if (! property->is_defined()) {
        property->set(value_in);
    }
}

} // namespace jackalope
