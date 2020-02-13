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

#include <chrono>
#include <iostream>
#include <string>

#include <jackalope/exception.h>
#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope;
using namespace jackalope::log;

int main(UNUSED int argc_in, UNUSED char ** argv_in)
{
    if (argc_in != 2) {
        jackalope_panic("must specify exactly one audio file to play");
    }

    auto dest = jackalope::make_shared<console_dest_t>(level_t::info);
    get_engine()->add_destination(dest);

    return(0);
}
