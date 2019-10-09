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

#include <jackalope/log/log_dest.h>
#include <jackalope/logging.h>

using namespace jackalope;

int main(void)
{
    auto dest = make_shared<log::log_console>(log::log_level::info);
    log::get_engine()->add_destination(dest);

    log_info("Hello ", 123);

    return(0);
}
