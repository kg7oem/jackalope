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

#include <jackalope/message.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

#define JACKALOPE_MESSAGE_OBJECT_INVOKE_SLOT "object.invoke_slot"

namespace jackalope {

struct signal_t;
struct slot_t;

using slot_function_t = function_t<void ()>;

// Thread safe with out user requirements
struct signal_t : public base_t, public shared_obj_t<signal_t>, lockable_t {
    const string_t name;
    pool_list_t<slot_function_t> connections;
    pool_list_t<promise_t<void>> waiters;

    signal_t(const string_t& name_in);
    void subscribe(slot_function_t handler_in);
    void subscribe(shared_t<slot_t> handler_in);
    void send();
    void wait();
};

// Thread safe because everything is const
struct slot_t : public base_t, public shared_obj_t<slot_t> {
    const string_t name;
    const slot_function_t handler;

    slot_t(const string_t& name_in, slot_function_t handler_in);
    void invoke();
};

class signal_obj_t {

protected:
    pool_map_t<string_t, shared_t<signal_t>> signals;
    pool_map_t<string_t, shared_t<slot_t>> slots;

public:
    virtual shared_t<signal_t> add_signal(const string_t& name_in);
    virtual shared_t<signal_t> get_signal(const string_t& name_in);
    virtual shared_t<slot_t> add_slot(const string_t& name_in, slot_function_t handler_in);
    virtual shared_t<slot_t> get_slot(const string_t& name_in);
};

} // namespace jackalope
