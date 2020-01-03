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
#include <jackalope/node.h>

namespace jackalope {

node_t::node_t(const init_list_t init_list_in)
: object_t(init_list_in), name(init_args_get(JACKALOPE_PROPERTY_NODE_NAME, init_args))
{
    assert(name != "");
}

void node_t::activate()
{
    assert_lockable_owner();

    object_t::activate();
}

void node_t::start()
{
    assert_lockable_owner();

    NODE_LOG(info, "Starting node");

    object_t::start();

    NODE_LOG(info, "Done starting node");
}

void node_t::stop()
{
    assert_lockable_owner();

    object_t::stop();
}

void node_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    assert_lockable_owner();

    object_t::deliver_one_message(message_in);

    run_if_needed();
}

// void node_t::source_available(shared_t<source_t> source_in)
// {
//     assert_lockable_owner();

//     object_t::source_available(source_in);

//     check_run_needed();
// }

// void node_t::sink_ready(shared_t<sink_t> sink_in)
// {
//     assert_lockable_owner();

//     object_t::sink_ready(sink_in);

//     check_run_needed();
// }

void node_t::run_if_needed()
{
    assert_lockable_owner();

    if (should_run()) {
        run();
    }
}

} //namespace jackalope
