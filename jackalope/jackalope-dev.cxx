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

int main(UNUSED int argc_in, UNUSED char ** argv_in)
{
    auto dest = jackalope::make_shared<console_dest_t>(level_t::trace);
    get_engine()->add_destination(dest);

    init();

    // auto system_audio = project->make_plugin("audio::jack");
    // for(auto i : { "audio.sample_rate", "audio.buffer_size" }) {
    //     project->add_variable(i, system_audio->peek(i));
    // }

    // log_info("buffer size: ", project->get_variable("audio.buffer_size"));

    auto project = project_t::make({
        { "audio.buffer_size", to_string(BUFFER_SIZE) },
        { "audio.sample_rate", to_string(SAMPLE_RATE) },
    });

    guard_object(project, {
        project->make_plugin({
            { JACKALOPE_PROPERTY_NODE_TYPE, "audio::gain" },
        });

        project->start();
        project->post_slot("object.stop");

        log_info("Waiting for project to stop");
        project->wait_stopped();
        log_info("Done waiting for project");

        project->shutdown();
    });

    shutdown();
    return(0);
}