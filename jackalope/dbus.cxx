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

DBus::Dispatcher::pointer dispatcher = nullptr;
DBus::Connection::pointer connection = nullptr;

void dbus_init() {
    assert(dispatcher == nullptr);
    assert(connection == nullptr);

    DBus::init();
    dispatcher = DBus::Dispatcher::create();
    connection = dispatcher->create_connection(DBus::BUS_SESSION);

    auto ret = connection->request_name(JACKALOPE_DBUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING);

    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        throw_runtime_error("duplicate DBus session name: ", JACKALOPE_DBUS_NAME);
    }
}

void dbus_register_object(DBus::Object::pointer object_in)
{
    assert(connection != nullptr);
    assert(dispatcher != nullptr);

    connection->register_object(object_in);
}

void dbus_unregister_object(DBus::Object::pointer object_in)
{
    assert(connection != nullptr);
    assert(dispatcher != nullptr);

    connection->register_object(object_in);
}

} //namespace jackalope
