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

#include <jackalope/exception.h>
#include <jackalope/dbus.h>

namespace jackalope {

static thread_t * dispatcher_thread = nullptr;
static DBus::BusDispatcher * global_dispatcher = nullptr;

void dbus_init() {
    assert(global_dispatcher == nullptr);

    global_dispatcher = new DBus::BusDispatcher();
    DBus::default_dispatcher = global_dispatcher;

    auto connection = dbus_get_connection();

    connection.request_name(JACKALOPE_DBUS_NAME_DEFAULT);

    dispatcher_thread = new thread_t([] { global_dispatcher->enter(); });
}

DBus::Connection& dbus_get_connection()
{
    static auto connection = DBus::Connection::SessionBus();
    return connection;
}

} //namespace jackalope
