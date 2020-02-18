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
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class property_t : public base_obj_t, protected lock_obj_t {

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
    bool defined_flag = false;

public:
    const type_t type = type_t::unknown;

    template <typename... Args>
    static shared_t<property_t> make(Args... args)
    {
        return jackalope::make_shared<property_t>(args...);
    }

    property_t(const type_t type_in);
    property_t(const type_t type_in, const double value_in);
    property_t(const type_t type_in, const string_t& value_in);
    ~property_t();

    bool is_defined();
    string_t get();
    void set(const double value_in);
    void set(const string_t& value_in);
    size_t& get_size();
    void set_size(const size_t size_in);
    int_t& get_integer();
    void set_integer(const int_t integer_in);
    void set_real(const real_t real_in);
    real_t& get_real();
    string_t get_string();
    void set_string(const string_t& string_in);
};

using prop_args_t = pool_vector_t<std::pair<const string_t, property_t::type_t>>;

class prop_obj_t {

protected:
    pool_map_t<string_t, shared_t<property_t>> properties;
    mutex_t property_mutex;
    virtual lock_t get_property_lock();
    virtual shared_t<property_t> _add_property(const string_t& name_in, const property_t::type_t type_in);
    virtual shared_t<property_t> _add_property(const string_t& name_in, const property_t::type_t type_in, const double value_in);
    virtual shared_t<property_t> _add_property(const string_t& name_in, const property_t::type_t type_in, const string_t& value_in);
    virtual shared_t<property_t> _get_property(const string_t& name_in);
    virtual void _add_properties(const prop_args_t& properties_in);
    virtual bool _has_property(const string_t& name_in);
    virtual void _set_property(const string_t& name_in, const double value_in);
    virtual void _set_undef_property(const string_t& name_in, const double value_in);
    virtual void _set_property(const string_t& name_in, const string_t& value_in);
    virtual void _set_undef_property(const string_t& name_in, const string_t& value_in);

    virtual shared_t<property_t> add_property(const string_t& name_in, const property_t::type_t type_in);
    virtual shared_t<property_t> add_property(const string_t& name_in, const property_t::type_t type_in, const double value_in);
    virtual shared_t<property_t> add_property(const string_t& name_in, const property_t::type_t type_in, const string_t& value_in);
    virtual void add_properties(const prop_args_t& properties_in);
    virtual void set_property(const string_t& name_in, const double value_in);
    virtual void set_undef_property(const string_t& name_in, const double value_in);
    virtual void set_property(const string_t& name_in, const string_t& value_in);
    virtual void set_undef_property(const string_t& name_in, const string_t& value_in);

public:
    virtual bool has_property(const string_t& name_in);
    virtual shared_t<property_t> get_property(const string_t& name_in);
    virtual pool_vector_t<string_t> get_property_names();
};

} // namespace jackalope
