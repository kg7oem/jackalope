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

#include <jackalope/signal.h>

namespace jackalope {

signal_t::signal_t(const string_t& name_in)
: name(name_in)
{ }

connection_t * signal_t::connect(slot_t * slot_in)
{
    auto connection = new connection_t(this, slot_in);
    connections.push_back(connection);
    return connection;
}

void signal_t::send()
{
    for(auto i : connections) {
        i->to->invoke(this);
    }
}

slot_t::slot_t(const string_t& name_in, slot_handler_t handler_in)
: name(name_in), handler(handler_in)
{ }

void slot_t::invoke(signal_t * from_in)
{
    handler(from_in);
}

connection_t::connection_t(signal_t * from_in, slot_t * to_in)
: from(from_in), to(to_in)
{ }

} // namespace jackalope
