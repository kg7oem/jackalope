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

#include <jackalope/audio.h>
#include <jackalope/network.h>
#include <jackalope/jackalope.h>

#ifdef CONFIG_HAVE_DBUS
#include <jackalope/dbus.h>
#endif

namespace jackalope {

void init()
{
#ifdef CONFIG_HAVE_DBUS
    jackalope::dbus_init();
#endif

    jackalope::network_init();

    jackalope::audio_init();
}

} // namespace jackalope
