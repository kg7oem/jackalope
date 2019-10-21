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

#include <jackalope/node.forward.h>
#include <jackalope/string.h>
#include <jackalope/types.h>

#define JACKALOPE_SIGNAL_FILE_EOF "file:eof"
#define JACKALOPE_SLOT_SYSTEM_TERMINATE "system:terminate"

namespace jackalope {

struct signal_t;
struct slot_t;
struct connection_t;

using slot_handler_t = function_t<void (signal_t * sender_in)>;

struct signal_t : public baseobj_t {
    const string_t name;
    pool_list_t<connection_t *> connections;

    signal_t(const string_t& name_in);
    connection_t * connect(slot_t * dest_in);
    void send();
};

struct slot_t : public baseobj_t {
    const string_t name;
    const slot_handler_t handler;

    slot_t(const string_t& name_in, slot_handler_t handler_in);
    void invoke(signal_t * sender_in);
};

struct connection_t : public baseobj_t {
    signal_t * from;
    slot_t * to;

    connection_t(signal_t * from_in, slot_t * to_in);
};

} // namespace jackalope