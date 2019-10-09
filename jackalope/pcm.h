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

#include <jackalope/component.h>

#define JACKALOPE_PCM_COMPONENT_TYPE "pcm"
#define JACKALOPE_PCM_REAL_TYPE "pcm[real]"
#define JACKALOPE_PCM_COMPLEX_TYPE "pcm[complex]"

namespace jackalope {

namespace pcm {

struct component_t : public jackalope::component_t {

public:
    template <class T>
    struct input_t : public jackalope::component_t::input_t {
        using sample_t = T;

        input_t(const string_t& name_in, component_t& parent_in)
        : jackalope::component_t::input_t(name_in, parent_in)
        { }
    };

    class real_input_t : public input_t<real_t> {

    public:
        real_input_t(const string_t& name_in, component_t& parent_in);
        virtual const string_t& get_type();
    };

    class complex_input_t : public input_t<complex_t> {
    public:
        complex_input_t(const string_t& name_in, component_t& parent_in);
        virtual const string_t& get_type();
    };

    template <class T>
    class output_t : public jackalope::component_t::output_t {
        using sample_t = T;
    };

    class real_output : public output_t<real_t> {

    public:
        virtual const string_t& get_type();
    };

    class complex_output : public output_t<complex_t> {

    public:
        virtual const string_t& get_type();
    };

    virtual jackalope::component_t::input_t& add_input(const string_t& type_in, const string_t& name_in);
    jackalope::component_t::input_t& add_real_input(const string_t& name_in);
    jackalope::component_t::input_t& add_complex_input(const string_t& name_in);
    const string_t& get_type();
};

} // namespace pcm

} // namespace jackalope
