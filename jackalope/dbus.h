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

#include <dbus-c++/dbus.h>

#include <dbus.adaptor.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_DBUS_NAME_PREFIX     "x-kg7oem.jackalope.server"
#define JACKALOPE_DBUS_NAME_DEFAULT    "x-kg7oem.jackalope.server.default"

namespace jackalope {

void dbus_init();
DBus::Connection& dbus_get_connection();

} //namespace jackalope
