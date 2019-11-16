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

#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>
#include <jackalope/pcm.h>

#define BUFFER_SIZE 128
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope;

int main(int argc_in, char ** argv_in)
{
    if (argc_in != 2) {
        jackalope_panic("must specify exactly one audio file to play");
    }

    auto dest = make_shared<log::console_dest_t>(log::level_t::info);
    log::get_engine()->add_destination(dest);

    jackalope_init();

    auto domain = make_pcm_domain({
        { "node:name", "main domain" },
        { "pcm:sample_rate", to_string(48000) },
        { "pcm:buffer_size", to_string(128) },
        { "input:left", "pcm[real]" },
        { "input:right", "pcm[real]" },
    });

    auto system_audio = domain->make_driver({
        { "node:class", "pcm::portaudio" },
        { "node:name", "system audio" },
    });

    auto input_file = domain->make_node({
        { "node:class", "pcm::sndfile" },
        { "node:name", "input file" },
        { "config:path", argv_in[1] },
    });

    auto left_tube = domain->make_node({
        { "node:class", "pcm::ladspa" },
        { "node:name", "left tube" },
        { "plugin:id", to_string(LADSPA_ZAMTUBE_ID) },
    });

    auto right_tube = domain->make_node({
        { "node:class", "pcm::ladspa" },
        { "node:name", "right tube" },
        { "plugin:id", to_string(LADSPA_ZAMTUBE_ID) },
    });

    input_file->get_signal("file:eof")->connect(domain->get_slot("system:terminate"));

    input_file->get_output("output 1")->link(left_tube->get_input("Audio Input 1"));
    input_file->get_output("output 1")->link(right_tube->get_input("Audio Input 1"));
    left_tube->get_output("Audio Output 1")->link(domain->get_input("left"));
    right_tube->get_output("Audio Output 1")->link(domain->get_input("right"));

    domain->activate();
    domain->start();

    system_audio->start();

    while(1) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    return(0);
}
