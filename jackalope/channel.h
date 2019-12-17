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

class channel_t;
class source_t;
class sink_t;

using source_constructor_t = function_t<shared_t<source_t> (const string_t& name_in, const string_t& type_in)>;
using sink_constructor_t = function_t<shared_t<sink_t> (const string_t& name_in, const string_t& type_in)>;

void add_source_constructor(const string_t& class_name_in, source_constructor_t constructor_in);
void add_sink_constructor(const string_t& class_name_in, sink_constructor_t constructor_in);
source_constructor_t get_source_constructor(const string_t& class_name_in);
sink_constructor_t get_sink_constructor(const string_t& class_name_in);

class channel_t : base_t {

protected:
    channel_t(const string_t& name_in, const string_t& type_in);

public:
    const string_t name;
    const string_t type;
};

class source_t : public channel_t {

public:
    static shared_t<source_t> make(const string_t& name_in, const string_t& type_in);
    source_t(const string_t& name_in, const string_t& type_in);
};

class sink_t : public channel_t {

public:
    static shared_t<sink_t> make(const string_t& name_in, const string_t& type_in);
    sink_t(const string_t& name_in, const string_t& type_in);
};

} // namespace jackalope
