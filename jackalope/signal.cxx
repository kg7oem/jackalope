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
#include <jackalope/object.h>
#include <jackalope/signal.h>

namespace jackalope {

invoke_slot_message_t::invoke_slot_message_t(const string_t& slot_name_in)
: message_t(invoke_slot_message_t::message_name, slot_name_in)
{
    assert(slot_name_in != "");
}

signal_t::subscription_t::subscription_t(shared_t<object_t> subscriber_in, const string_t& slot_name_in)
: subscriber(subscriber_in), slot_name(slot_name_in)
{
    assert(subscriber != nullptr);
    assert(slot_name_in != "");
}

signal_t::signal_t(const string_t& name_in)
: name(name_in)
{ }

signal_t::~signal_t()
{
    assert(shutdown_flag);
}

void signal_t::shutdown()
{
    auto lock = get_object_lock();

    if (shutdown_flag) {
        return;
    }

    subscriptions.empty();

    shutdown_flag = true;
}

void signal_t::subscribe(shared_t<object_t> object_in, const string_t& name_in)
{
    auto lock = get_object_lock();
    subscriptions.emplace(subscriptions.end(), object_in, name_in);
}

void signal_t::send()
{
    auto lock = get_object_lock();

    for(auto& i : waiters) {
        i.set_value();
    }

    waiters.empty();

    for (auto i = subscriptions.begin(); i != subscriptions.end(); i++) {
        try {
            auto subscriber = i->subscriber;
            subscriber->send_message<invoke_slot_message_t>(i->slot_name);
        } catch (const std::bad_weak_ptr&) {
            subscriptions.erase(i);
            continue;
        }
    }
}

void signal_t::wait()
{
    future_t<void> wakeup_future;

    guard_lockable({
        auto& wakeup_promise = waiters.emplace_back();
        wakeup_future = wakeup_promise.get_future();
    });

   wakeup_future.get();
}

slot_t::slot_t(const string_t& name_in, const slot_function_t handler_in)
: name(name_in), handler(handler_in)
{
    assert(name != "");
}

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

void signal_obj_t::send_signal(const string_t& name_in)
{
    auto signal = get_signal(name_in);
    signal->send();
}

shared_t<slot_t> signal_obj_t::add_slot(shared_t<slot_t> slot_in)
{
    auto name = slot_in->name;

    if (slots.find(name) != slots.end()) {
        throw_runtime_error("Duplicate slot name: ", name);
    }

    slots.insert({ name, slot_in });
    return slot_in;
}

shared_t<slot_t> signal_obj_t::add_slot(const string_t& name_in, slot_function_t handler_in)
{
    auto new_slot = jackalope::make_shared<slot_t>(name_in, handler_in);

    return add_slot(new_slot);
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
