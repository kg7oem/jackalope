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
#include <jackalope/node.h>
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

    auto graph = make_graph({
        { JACKALOPE_PCM_PROPERTY_SAMPLE_RATE, to_string(SAMPLE_RATE) },
        { JACKALOPE_PCM_PROPERTY_BUFFER_SIZE, to_string(BUFFER_SIZE) },
    });

    // auto driver = graph->add_object({
    //     { "object.class", "pcm::portaudio" },
    //     { "source.left", "pcm[real]" },
    //     { "source.right", "pcm[real]" },
    //     { "sink.left", "pcm[real]" },
    //     { "sink.right", "pcm[real]" },
    // });

    auto input_file = graph->add_node({
        { "foo", "bar " },
        { JACKALOPE_PROPERTY_NODE_CLASS, "pcm::sndfile" },
        { JACKALOPE_PROPERTY_NODE_NAME, "input file" },
        { "config.path", argv_in[1] },
    });

    // auto left_tube = graph->add_object({
    //     { "object.class", "pcm::ladspa" },
    //     { "object.name", "left tube" },
    //     { "plugin.id", to_string(LADSPA_ZAMTUBE_ID) },
    // });

    // auto right_tube = graph->add_object({
    //     { "object.class", "pcm::ladspa" },
    //     { "object.name", "right tube" },
    //     { "plugin.id", to_string(LADSPA_ZAMTUBE_ID) },
    // });

    input_file->connect(JACKALOPE_SIGNAL_FILE_EOF, graph, JACKALOPE_OBJECT_SLOT_STOP);

    // input_file->link("output 1", left_tube, "Audio Input 1");
    // input_file->link("output 1", right_tube, "Audio Input 1");
    // left_tube->link("Audio Output 1", driver, "left");
    // right_tube->link("Audio Output 1", driver, "right");

    graph->start();

    log_info("after start() was called");

    graph->wait_stop();

    log_info("after wait_stop() was called");

    jackalope_shutdown();

    return(0);
}
