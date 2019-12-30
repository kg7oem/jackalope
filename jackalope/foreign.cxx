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
#include <jackalope/foreign.h>

namespace jackalope {

namespace foreign {

source_t::source_t(shared_t<jackalope::source_t> wrapped_in)
: wrapper_t(wrapped_in)
{ }

sink_t::sink_t(shared_t<jackalope::sink_t> wrapped_in)
: wrapper_t(wrapped_in)
{ }

node_t::node_t(shared_t<jackalope::node_t> wrapped_in)
: wrapper_t(wrapped_in)
{ }

source_t node_t::add_source(const string_t& name_in)
{
    auto new_source = wait_job<shared_t<jackalope::source_t>>([&] {
        auto lock = wrapped->get_object_lock();
        return wrapped->add_source(name_in);
    });

    return source_t(new_source);
}

sink_t node_t::add_sink(const string_t& name_in)
{
    auto new_sink = wait_job<shared_t<jackalope::sink_t>>([&] {
        auto lock = wrapped->get_object_lock();
        return wrapped->add_sink(name_in);
    });

    return sink_t(new_sink);
}

void node_t::start()
{
    wait_job<void>([&] {
        auto lock = wrapped->get_object_lock();
        wrapped->start();
    });
}

} // namespace foreign

} //namespace jackalope
