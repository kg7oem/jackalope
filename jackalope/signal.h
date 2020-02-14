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

#include <jackalope/forward.h>
#include <jackalope/message.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

using slot_function_t = function_t<void ()>;

struct invoke_slot_message_t : public message_t<const string_t> {
    inline static const string_t message_name = "object.invoke_slot";
    invoke_slot_message_t(const string_t& slot_name_in);
};

// Thread safe with out user requirements
struct signal_t : public shared_obj_t<signal_t>, lock_obj_t {

public:
    struct subscription_t {
        const weak_t<object_t> weak_subscriber;
        const string_t slot_name;

        subscription_t(shared_t<object_t> subscriber_in, const string_t& slot_name_in);
    };

protected:
    pool_list_t<subscription_t> subscriptions;
    pool_list_t<promise_t<void>> waiters;

public:
    const string_t name;

    signal_t(const string_t& name_in);
    void subscribe(shared_t<object_t> object_in, const string_t& name_in);
    void send();
    void wait();
};

// Thread safe because everything is const
struct slot_t : public shared_obj_t<slot_t> {
    const string_t name;
    const slot_function_t handler;

    slot_t(const string_t& name_in, const slot_function_t handler_in);
    virtual void invoke();
};

class signal_obj_t {

protected:
    pool_map_t<string_t, shared_t<signal_t>> signals;
    pool_map_t<string_t, shared_t<slot_t>> slots;

    virtual shared_t<signal_t> add_signal(const string_t& name_in);
    virtual shared_t<slot_t> add_slot(shared_t<slot_t> slot_in);
    virtual shared_t<slot_t> add_slot(const string_t& name_in, slot_function_t handler_in);

public:
    virtual shared_t<signal_t> get_signal(const string_t& name_in);
    virtual shared_t<slot_t> get_slot(const string_t& name_in);
};

} // namespace jackalope
