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

#define BUFFER_SIZE 512
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope::log;

jackalope_network_t make_tube_simulator(jackalope_graph_t& graph_in);

int main(int argc_in, char ** argv_in)
{
    if (argc_in != 2) {
        jackalope_panic("must specify exactly one audio file to play");
    }

    auto dest = jackalope::make_shared<console_dest_t>(level_t::info);
    get_engine()->add_destination(dest);

    jackalope_init();

    auto graph = jackalope_graph_t::make({
        { "pcm.sample_rate", jackalope::to_string(SAMPLE_RATE) },
        { "pcm.buffer_size", jackalope::to_string(BUFFER_SIZE) },
    });

    auto input_file = graph.make_node({
        { "object.type", "audio::sndfile" },
        { "node.name", "input file" },
        { "config.path", argv_in[1] },
    });

    auto system_audio = graph.make_node({
        { "object.type", "audio::portaudio" },
        { "node.name", "system audio" },
        { "sink.left", "audio" },
        { "sink.right", "audio" },
    });

    auto tube_simulator = make_tube_simulator(graph);

    input_file.subscribe(JACKALOPE_SIGNAL_OBJECT_STOPPED, graph, JACKALOPE_SLOT_OBJECT_STOP);

    input_file.link("Output 1", tube_simulator, "left");
    input_file.link("Output 1", tube_simulator, "right");

    tube_simulator.link("left", system_audio, "left");
    tube_simulator.link("right", system_audio, "right");

    tube_simulator.poke("config.drive", 0);
    tube_simulator.poke("config.gain", -3);

    graph.run();

    return(0);
}

jackalope_network_t make_tube_simulator(jackalope_graph_t& graph_in)
{
    auto tube_simulator = graph_in.make_network({
        { "node.name", "tube simulator" },
    });

    std::map<jackalope::string_t, jackalope::string_t> tube_property_name_map({
        { "config.Tube Drive", "config.drive" },
        { "config.Bass", "config.bass" },
        { "config.Mids", "config.mids" },
        { "config.Treble", "config.treble" },
    });

    for(auto property_name : { "config.gain", "config.drive", "config.bass", "config.mids", "config.treble" }) {
        tube_simulator.add_property(property_name, jackalope::property_t::type_t::real);
    }

    for(auto i : { "left", "right" }) {
        auto tube_node = tube_simulator.make_node({
            { "object.type", "audio::ladspa" },
            { "node.activate", "false" },
            { "node.name", jackalope::to_string(i, " tube") },
            { "plugin.id", jackalope::to_string(LADSPA_ZAMTUBE_ID) },
        });

        auto gain_node = tube_simulator.make_node({
            { "object.type", "audio::gain" },
            { "node.activate", "false" },
            { "node.name", jackalope::to_string(i, " gain") },
            { "pcm.sample_rate", jackalope::to_string(SAMPLE_RATE) },
            { "pcm.buffer_size", jackalope::to_string(BUFFER_SIZE) },
        });

        tube_simulator.add_source(i, "audio");
        tube_simulator.add_sink(i, "audio");

        for(auto i : tube_property_name_map) {
            tube_node.alias_property(i.first, tube_simulator, i.second);
        }

        gain_node.alias_property("config.gain", tube_simulator, "config.gain");

        tube_node.activate();
        gain_node.activate();

        tube_simulator.forward(i, tube_node, "Audio Input 1");
        tube_node.link("Audio Output 1", gain_node, "input");
        gain_node.forward("output", tube_simulator, i);
    }

    return tube_simulator;
}
