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
#include <jackalope/jackalope.h>
#include <jackalope/pcm.h>
#include <jackalope/pcm/ladspa.h>
// #include <jackalope/pcm/portaudio.h>
#include <jackalope/pcm/sndfile.h>

void jackalope_init()
{
    jackalope::async_init();

    jackalope::pcm_init();
    jackalope::pcm::ladspa_init();
    // jackalope::pcm::portaudio_init();
    jackalope::pcm::sndfile_init();
}

void jackalope_shutdown()
{
    jackalope::async_shutdown();
}
