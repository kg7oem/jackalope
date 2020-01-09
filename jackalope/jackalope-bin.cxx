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

#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

#define BUFFER_SIZE 256
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope::log;

int main(int argc_in, char ** argv_in)
{
    if (argc_in != 2) {
        jackalope_panic("must specify exactly one audio file to play");
    }

    auto dest = jackalope::make_shared<console_dest_t>(level_t::info);
    get_engine()->add_destination(dest);

    jackalope_init();

    auto connection_manager = jackalope_daemon_t::make("audio::jackaudio::connections", {
        { "jackalope:left", "system:playback_1" },
        { "jackalope:right", "system:playback_2" },
    });

    connection_manager.start();

    auto graph = jackalope_graph_t::make({
        { "pcm.buffer_size", jackalope::property_t::type_t::size },
        { "pcm.sample_rate", jackalope::property_t::type_t::size },
    });

    auto system_audio = graph.add_node({
        { "object.type", "audio::jackaudio" },
        { "node.name", "system audio" },
        { "pcm.buffer_size", jackalope::to_string(BUFFER_SIZE) },
        { "pcm.sample_rate", jackalope::to_string(SAMPLE_RATE) },
        { "sink.left", "audio" },
        { "sink.right", "audio" },
        { "config.client_name", "jackalope" },
    });

    graph.poke("pcm.sample_rate", system_audio.peek("pcm.sample_rate"));
    graph.poke("pcm.buffer_size", system_audio.peek("pcm.buffer_size"));

    auto input_file = graph.add_node({
        { "object.type", "audio::sndfile" },
        { "node.name", "input file" },
        { "config.path", argv_in[1] },
    });

    auto left_tube = graph.add_node({
        { "object.type", "audio::ladspa" },
        { "node.name", "left tube" },
        { "plugin.id", jackalope::to_string(LADSPA_ZAMTUBE_ID) },
    });

    auto right_tube = graph.add_node({
        { "object.type", "audio::ladspa" },
        { "node.name", "right tube" },
        { "plugin.id", jackalope::to_string(LADSPA_ZAMTUBE_ID) },
    });

    input_file.connect(JACKALOPE_SIGNAL_OBJECT_STOPPED, graph, JACKALOPE_SLOT_OBJECT_STOP);

    input_file.link("Output 1", left_tube, "Audio Input 1");
    input_file.link("Output 1", right_tube, "Audio Input 1");
    left_tube.link("Audio Output 1", system_audio, "left");
    right_tube.link("Audio Output 1", system_audio, "right");

    graph.run();

    connection_manager.stop();

    log_info("shutting down");
    jackalope_shutdown();
    log_info("done shutting down");

    return(0);
}
