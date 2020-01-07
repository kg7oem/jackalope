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

#include <dbus-cxx.h>

#include <jackalope/dbus_objectAdaptee.h>
#include <jackalope/dbus_objectAdapter.h>
#include <jackalope/types.h>

#define JACKALOPE_DBUS_NAME "x-kg7oem.jackalope.server"

namespace jackalope {

void dbus_init();
void dbus_register_object(DBus::Object::pointer object_in);
void dbus_unregister_object(DBus::Object::pointer object_in);

} //namespace jackalope
