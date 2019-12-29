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

#include <jackalope/node.h>

namespace jackalope {

void node_t::slot_sink_ready(shared_t<sink_t>)
{
    auto lock = get_object_lock();
    // TODO handle sinks being ready
}

void node_t::slot_source_available(shared_t<source_t>) {
    auto lock = get_object_lock();
    // TODO handle sources being available
}

} //namespace jackalope
