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
    log_info("adding ", type_in, " input named ", name_in);

    // FIXME placeholde
    return * static_cast<jackalope::component::input *>(nullptr);
}

const string_type& component::real_input::get_type()
{
    static const string_type type(JACKALOPE_PCM_REAL_TYPE);
    return type;
}

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

const string_type& component::complex_output::get_type()
{
    static const string_type type(JACKALOPE_PCM_COMPLEX_TYPE);
    return type;
}

} // namespace pcm

} // namespace jackalope
