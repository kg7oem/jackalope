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

#include <iostream>
#include <string>

#include <jackalope/log/dest.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>
#include <jackalope/pcm.h>

using namespace jackalope;

int main(void)
{
    auto dest = make_shared<log::console_dest_t>(log::level_t::info);
    log::get_engine()->add_destination(dest);

    node_t foo;
    foo.add_component<pcm::component_t>();
    foo.add_input("pcm[real]", "test");
    foo.add_input("pcm[complex]", "blaz");

    log_info("Hello ", 123);

    return(0);
}
