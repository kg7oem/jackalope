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
#include <jackalope/module.h>
#include <jackalope/node.h>
#include <jackalope/project.h>

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace std::chrono_literals;
using namespace jackalope;
using namespace jackalope::log;

int main(int argc_in, char ** argv_in)
{
    if (argc_in != 2) {
        jackalope_panic("must specify a filename to play");
    }

    auto dest = jackalope::make_shared<console_dest_t>(level_t::trace);
    get_engine()->add_destination(dest);

    init();

    auto project = project_t::make({
        { "audio.buffer_size", to_string(BUFFER_SIZE) },
    });

    guard_object(project, {
        auto file = project->make_plugin({
            { JACKALOPE_PROPERTY_NODE_TYPE, "audio::sndfile" },
            { "config.path", argv_in[1] },
        });

        project->add_variable("audio.sample_rate", file->get_property("audio.sample_rate")->get());

        // auto gain = project->make_plugin({
        //     { JACKALOPE_PROPERTY_NODE_TYPE, "audio::gain" },
        // });

        project->start();

        log_info("Waiting for project to stop");
        project->wait_stopped();
        log_info("Done waiting for project");

        project->shutdown();
    });

    shutdown();
    return(0);
}
