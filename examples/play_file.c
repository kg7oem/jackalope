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

    jackalope_object_connect(input_file, "object.stopped", graph, "object.stop");
    jackalope_object_link(input_file, "Output 1", left_tube, "Audio Input 1");
    jackalope_object_link(input_file, "Output 1", right_tube, "Audio Input 1");
    jackalope_object_link(left_tube, "Audio Output 1", system_audio, "left");
    jackalope_object_link(right_tube, "Audio Output 1", system_audio, "right");

    jackalope_graph_run(graph);

    jackalope_object_delete(right_tube);
    jackalope_object_delete(left_tube);
    jackalope_object_delete(system_audio);
    jackalope_object_delete(input_file);
    jackalope_object_delete(graph);

    jackalope_shutdown();

    return 0;
}