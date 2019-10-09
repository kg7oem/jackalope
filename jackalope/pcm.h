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

struct component : public jackalope::component {

public:
    template <class T>
    struct input : public jackalope::component::input {
        using sample_type = T;

        input(const string_type& name_in, component& parent_in)
        : jackalope::component::input(name_in, parent_in)
        { }
    };

    class real_input : public input<real_t> {

    public:
        real_input(const string_type& name_in, component& parent_in);
        virtual const string_type& get_type();
    };

    class complex_input : public input<complex_t> {
    public:
        complex_input(const string_type& name_in, component& parent_in);
        virtual const string_type& get_type();
    };

    template <class T>
    class output : public jackalope::component::output {
        using sample_type = T;
    };

    class real_output : public output<real_t> {

    public:
        virtual const string_type& get_type();
    };

    class complex_output : public output<complex_t> {

    public:
        virtual const string_type& get_type();
    };

    virtual jackalope::component::input& add_input(const string_type& type_in, const string_type& name_in);
    jackalope::component::input& add_real_input(const string_type& name_in);
    jackalope::component::input& add_complex_input(const string_type& name_in);
    const string_type& get_type();
};

} // namespace pcm

} // namespace jackalope
