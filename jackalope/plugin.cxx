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

#include <jackalope/plugin.h>

namespace jackalope {

plugin_t::plugin_t(const init_args_t init_args_in)
: node_t(init_args_in)
{ }

void plugin_t::start()
{
    assert_lockable_owner();

    node_t::start();

    execute_if_needed();
}

void plugin_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    object_t::deliver_one_message(message_in);

    auto lock = get_object_lock();

    if (started_flag) {
        execute_if_needed();
    }
}

void plugin_t::execute_if_needed()
{
    assert_lockable_owner();

    assert(started_flag);

    while(1) {
        if (stopped_flag || ! should_execute()) {
            break;
        }

        execute();
    }
}

driver_plugin_t::driver_plugin_t(const init_args_t init_args_in)
: plugin_t(init_args_in)
{ }

bool driver_plugin_t::should_execute()
{
    assert_lockable_owner();

    for (auto i : sinks) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

threaded_driver_plugin_t::threaded_driver_plugin_t(const init_args_t init_args_in)
: driver_plugin_t(init_args_in)
{ }

bool threaded_driver_plugin_t::should_execute()
{
    assert_lockable_owner();

    if (driver_thread_run_flag) {
        return false;
    }

    return driver_plugin_t::should_execute();
}

void threaded_driver_plugin_t::execute() {
    assert_lockable_owner();

    assert(started_flag);
    assert(! driver_thread_run_flag);

    driver_thread_run_flag = true;
    driver_thread_cond.notify_all();
}

void threaded_driver_plugin_t::stop()
{
    assert_lockable_owner();

    driver_plugin_t::stop();

    driver_thread_cond.notify_all();
    driver_thread_cond.wait(object_mutex, [&] { return driver_thread_run_flag == false; });
}

filter_plugin_t::filter_plugin_t(const init_args_t init_args_in)
: plugin_t(init_args_in)
{ }

bool filter_plugin_t::should_execute()
{
    assert_lockable_owner();

    for (auto i : sources) {
        if (! i->is_available()) {
            return false;
        }
    }

    for (auto i : sinks) {
        if (! i->is_ready()) {
            return false;
        }
    }

    return true;
}

} //namespace jackalope
