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

class channel_t : baseobj_t {

protected:
    const string_t name;
    const string_t type;

    channel_t(const string_t& name_in, const string_t& type_in);
};

class source_t : public channel_t {

public:
    source_t(const string_t& name_in, const string_t& type_in);
};

class sink_t : public channel_t {

public:
    sink_t(const string_t& name_in, const string_t& type_in);

};

} // namespace jackalope
