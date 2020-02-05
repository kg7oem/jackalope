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

#include <stdio.h>
#include <string.h>

#include <jackalope/foreign.h>

int main(const int argc_in, const char ** argv_in)
{

    if (argc_in != 2) {
        fprintf(stderr, "must specify a file to play\n");
        return(1);
    }

    jackalope_init();

    const char * graph_args[] = {
        "pcm.sample_rate", "48000",
        "pcm.buffer_size", "512",
        NULL
    };

    struct jackalope_object_t * graph = jackalope_graph_make(graph_args);

    const char * input_file_args[] = {
        "object.type", "audio::sndfile",
        "node.name", "input file",
        "config.path", argv_in[1],
        NULL
    };

    struct jackalope_object_t * input_file = jackalope_graph_add_node(graph, input_file_args);

    const char * system_audio_args[] = {
        "object.type", "audio::portaudio",
        "node.name", "system audio",
        "sink.left", "audio",
        "sink.right", "audio",
        NULL
    };

    struct jackalope_object_t * system_audio = jackalope_graph_add_node(graph, system_audio_args);

    const char * left_tube_args[] = {
        "object.type", "audio::ladspa",
        "node.name", "left tube",
        "plugin.id", "1515476290",
        NULL
    };

    struct jackalope_object_t * left_tube = jackalope_graph_add_node(graph, left_tube_args);

    const char * right_tube_args[] = {
        "object.type", "audio::ladspa",
        "node.name", "right tube",
        "plugin.id", "1515476290",
        NULL
    };

    struct jackalope_object_t * right_tube = jackalope_graph_add_node(graph, right_tube_args);

    jackalope_object_subscribe(input_file, "object.stopped", graph, "object.stop");
    jackalope_node_link(input_file, "Output 1", left_tube, "Audio Input 1");
    jackalope_node_link(input_file, "Output 1", right_tube, "Audio Input 1");
    jackalope_node_link(left_tube, "Audio Output 1", system_audio, "left");
    jackalope_node_link(right_tube, "Audio Output 1", system_audio, "right");

    jackalope_graph_run(graph);

    jackalope_object_delete(right_tube);
    jackalope_object_delete(left_tube);
    jackalope_object_delete(system_audio);
    jackalope_object_delete(input_file);
    jackalope_object_delete(graph);

    return 0;
}
