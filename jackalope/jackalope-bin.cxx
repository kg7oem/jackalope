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
#include <jackalope/object.h>

#define BUFFER_SIZE 128
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope::foreign;
using namespace jackalope::log;

int main(int argc_in, char **)
{
    if (argc_in != 2) {
        jackalope_panic("must specify exactly one audio file to play");
    }

    auto dest = jackalope::make_shared<console_dest_t>(level_t::info);
    get_engine()->add_destination(dest);

    jackalope_init();

    auto test = make_node({
        { "node.type", "audio::sndfile"},
        { "node.name", "test" },
    });

    for(auto i : { test }) {
        i.start();
    }

    // auto graph = make_graph({
    //     { JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, to_string(SAMPLE_RATE) },
    //     { JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, to_string(BUFFER_SIZE) },
    // });

    // auto driver = graph->add_object({
    //     { "object.class", "pcm::portaudio" },
    //     { "source.left", "pcm[real]" },
    //     { "source.right", "pcm[real]" },
    //     { "sink.left", "pcm[real]" },
    //     { "sink.right", "pcm[real]" },
    // });

    // auto input_file = graph->add_node({
    //     { JACKALOPE_PROPERTY_NODE_CLASS, "pcm::sndfile" },
    //     { JACKALOPE_PROPERTY_NODE_NAME, "input file" },
    //     { "config.path", argv_in[1] },
    //     { "source.left", JACKALOPE_CHANNEL_TYPE_PCM_REAL },
    //     { "source.right", JACKALOPE_CHANNEL_TYPE_PCM_REAL },
    // });

    // auto left_tube = graph->add_node({
    //     { JACKALOPE_PROPERTY_NODE_CLASS, "pcm::ladspa" },
    //     { JACKALOPE_PROPERTY_NODE_NAME, "left tube" },
    //     { "plugin.id", to_string(LADSPA_ZAMTUBE_ID) },
    // });

    // auto right_tube = graph->add_node({
    //     { JACKALOPE_PROPERTY_NODE_CLASS, "pcm::ladspa" },
    //     { JACKALOPE_PROPERTY_NODE_NAME, "right tube" },
    //     { "plugin.id", to_string(LADSPA_ZAMTUBE_ID) },
    // });

    // input_file->connect(JACKALOPE_SIGNAL_FILE_EOF, graph, JACKALOPE_SLOT_OBJECT_STOP);

    // input_file->link("left", left_tube, "Audio Input 1");
    // input_file->link("right", right_tube, "Audio Input 1");
    // left_tube->link("Audio Output 1", driver, "left");
    // right_tube->link("Audio Output 1", driver, "right");

    // graph->start();

    // log_info("after start() was called");

    // graph->wait_signal("object.stopped");

    // log_info("after wait_stop() was called");

    jackalope_shutdown();

    return(0);
}
