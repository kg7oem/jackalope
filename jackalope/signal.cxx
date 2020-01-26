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
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/signal.h>

namespace jackalope {

signal_t::signal_t(const string_t& name_in)
: name(name_in)
{ }

void signal_t::subscribe(slot_function_t handler_in)
{
    auto lock = get_object_lock();
    connections.push_back(handler_in);
}

void signal_t::subscribe(shared_t<slot_t> handler_in)
{
    auto lock = get_object_lock();
    connections.push_back([handler_in] { handler_in->invoke(); });
}

void signal_t::send()
{
    auto lock = get_object_lock();

    for(auto& i : waiters) {
        i.set_value();
    }

    waiters.empty();

    for (auto i : connections) {
        i();
    }
}

void signal_t::wait()
{
    future_t<void> wakeup_future;

    {
        auto lock = get_object_lock();
        auto& wakeup_promise = waiters.emplace_back();
        wakeup_future = wakeup_promise.get_future();
    }

   wakeup_future.get();
}

slot_t::slot_t(const string_t& name_in, slot_function_t handler_in)
: name(name_in), handler(handler_in)
{ }

void slot_t::invoke()
{
    handler();
}

shared_t<signal_t> signal_obj_t::add_signal(const string_t& name_in)
{
    if (signals.find(name_in) != signals.end()) {
        throw_runtime_error("Duplicate signal name: ", name_in);
    }

    auto signal = jackalope::make_shared<signal_t>(name_in);
    signals.insert({ name_in, signal });

    return signal;
}

shared_t<signal_t> signal_obj_t::get_signal(const string_t& name_in)
{
    auto found = signals.find(name_in);

    if (found == signals.end()) {
        throw_runtime_error("Could not find a signal: ", name_in);
    }

    return found->second;
}

shared_t<slot_t> signal_obj_t::add_slot(const string_t& name_in, slot_function_t handler_in)
{
    if (slots.find(name_in) != slots.end()) {
        throw_runtime_error("Duplicate slot name: ", name_in);
    }

    auto new_slot = jackalope::make_shared<slot_t>(name_in, handler_in);
    slots.insert({ name_in, new_slot });

    return new_slot;
}

shared_t<slot_t> signal_obj_t::get_slot(const string_t& name_in)
{
    auto found = slots.find(name_in);

    if (found == slots.end()) {
        throw_runtime_error("could not find a slot: ", name_in);
    }

    return found->second;
}

} // namespace jackalope
