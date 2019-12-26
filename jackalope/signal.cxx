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

#include <jackalope/async.h>
#include <jackalope/logging.h>
#include <jackalope/signal.h>

namespace jackalope {

signal_t::signal_t(const string_t& name_in)
: name(name_in)
{ }

void signal_t::connect(shared_t<slot_t> slot_in)
{
    auto lock = get_object_lock();
    connections.push_back(slot_in);
}

void signal_t::send()
{
    auto lock = get_object_lock();
    auto shared_this = shared_obj<signal_t>();

    for (auto i : connections) {
        submit_job([i, shared_this] { i->invoke(shared_this); });
    }
}

slot_t::slot_t(const string_t& name_in, slot_handler_t handler_in)
: name(name_in), handler(handler_in)
{ }

void slot_t::invoke(shared_t<signal_t> from_in)
{
    handler(from_in);
}

} // namespace jackalope
