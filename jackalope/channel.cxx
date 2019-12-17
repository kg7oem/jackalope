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

#include <jackalope/channel.h>

namespace jackalope {

channel_t::channel_t(const string_t& name_in, const string_t& type_in)
: name(name_in), type(type_in)
{ }

source_t::source_t(const string_t& name_in, const string_t& type_in)
: channel_t(name_in, type_in)
{ }

sink_t::sink_t(const string_t& name_in, const string_t& type_in)
: channel_t(name_in, type_in)
{ }

} // namespace jackalope
