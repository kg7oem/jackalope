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

const string_type& component::get_type()
{
    static const string_type type(JACKALOPE_PCM_COMPONENT_TYPE);
    return type;
}

jackalope::component::input& component::add_input(const string_type& type_in, const string_type& name_in)
{
    auto found = inputs.find(name_in);
    auto subtype_name = jackalope::component::extract_component_extra(type_in);

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

jackalope::component::input& component::add_real_input(const string_type& name_in)
{
    auto new_input = new component::real_input(name_in, *this);

    inputs[name_in] = new_input;
    return *new_input;
}

jackalope::component::input& component::add_complex_input(const string_type& name_in)
{
    auto new_input = new component::complex_input(name_in, *this);

    inputs[name_in] = new_input;
    return *new_input;
}

const string_type& component::real_input::get_type()
{
    static const string_type type(JACKALOPE_PCM_REAL_TYPE);
    return type;
}

component::real_input::real_input(const string_type& name_in, component& parent_in)
: input<sample_type>(name_in, parent_in)
{ }

const string_type& component::real_output::get_type()
{
    static const string_type type(JACKALOPE_PCM_REAL_TYPE);
    return type;
}

const string_type& component::complex_input::get_type()
{
    static const string_type type(JACKALOPE_PCM_COMPLEX_TYPE);
    return type;
}

component::complex_input::complex_input(const string_type& name_in, component& parent_in)
: input<sample_type>(name_in, parent_in)
{ }

const string_type& component::complex_output::get_type()
{
    static const string_type type(JACKALOPE_PCM_COMPLEX_TYPE);
    return type;
}

} // namespace pcm

} // namespace jackalope
