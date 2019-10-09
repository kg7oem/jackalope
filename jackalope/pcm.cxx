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
#include <jackalope/pcm.h>

namespace jackalope {

namespace pcm {

const string_t& component_t::get_type()
{
    static const string_t type(JACKALOPE_PCM_COMPONENT_TYPE);
    return type;
}

jackalope::component_t::input_t& component_t::add_input(const string_t& type_in, const string_t& name_in)
{
    auto found = inputs.find(name_in);
    auto subtype_name = jackalope::component_t::extract_component_extra(type_in);

    if (found != inputs.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    if (subtype_name == "") {
        throw_runtime_error("could not parse input type: ", type_in);
    } else if (subtype_name == "real") {
        return add_real_input(name_in);
    } else if (subtype_name == "complex") {
        return add_complex_input(name_in);
    }

    throw_runtime_error("unknown pcm subtype: ", subtype_name);
}

jackalope::component_t::input_t& component_t::add_real_input(const string_t& name_in)
{
    auto new_input = new component_t::real_input_t(name_in, *this);

    inputs[name_in] = new_input;
    return *new_input;
}

jackalope::component_t::input_t& component_t::add_complex_input(const string_t& name_in)
{
    auto new_input = new component_t::complex_input_t(name_in, *this);

    inputs[name_in] = new_input;
    return *new_input;
}

const string_t& component_t::real_input_t::get_type()
{
    static const string_t type(JACKALOPE_PCM_REAL_TYPE);
    return type;
}

component_t::real_input_t::real_input_t(const string_t& name_in, component_t& parent_in)
: input_t<sample_type_t_t_t>(name_in, parent_in)
{ }

const string_t& component_t::real_output::get_type()
{
    static const string_t type(JACKALOPE_PCM_REAL_TYPE);
    return type;
}

const string_t& component_t::complex_input_t::get_type()
{
    static const string_t type(JACKALOPE_PCM_COMPLEX_TYPE);
    return type;
}

component_t::complex_input_t::complex_input_t(const string_t& name_in, component_t& parent_in)
: input_t<sample_type_t_t_t>(name_in, parent_in)
{ }

const string_t& component_t::complex_output::get_type()
{
    static const string_t type(JACKALOPE_PCM_COMPLEX_TYPE);
    return type;
}

} // namespace pcm

} // namespace jackalope
