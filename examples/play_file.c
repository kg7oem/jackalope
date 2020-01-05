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

    struct jackalope_graph_t * graph = jackalope_graph_make(graph_args);

    const char * input_file_args[] = {
        "object.type", "audio::sndfile",
        "node.name", "input file",
        "config.path", argv_in[1],
        NULL
    };

    struct jackalope_node_t * input_file = jackalope_graph_add_node(graph, input_file_args);

    const char * system_audio_args[] = {
        "object.type", "audio::portaudio",
        "node.name", "system audio",
        "sink.left", "audio",
        "sink.right", "audio",
        NULL
    };

    struct jackalope_node_t * system_audio = jackalope_graph_add_node(graph, system_audio_args);

    const char * left_tube_args[] = {
        "object.type", "audio::ladspa",
        "node.name", "left tube",
        "plugin.id", "1515476290",
        NULL
    };

    struct jackalope_node_t * left_tube = jackalope_graph_add_node(graph, left_tube_args);

    const char * right_tube_args[] = {
        "object.type", "audio::ladspa",
        "node.name", "right tube",
        "plugin.id", "1515476290",
        NULL
    };

    struct jackalope_node_t * right_tube = jackalope_graph_add_node(graph, right_tube_args);

    jackalope_node_connect(input_file, "object.stopped", (struct jackalope_object_t *)graph, "object.stop");
    jackalope_node_link(input_file, "Output 1", (struct jackalope_object_t *)left_tube, "Audio Input 1");
    jackalope_node_link(input_file, "Output 1", (struct jackalope_object_t *)right_tube, "Audio Input 1");
    jackalope_node_link(left_tube, "Audio Output 1", (struct jackalope_object_t *)system_audio, "left");
    jackalope_node_link(right_tube, "Audio Output 1", (struct jackalope_object_t *)system_audio, "right");

    jackalope_graph_run(graph);

    jackalope_node_delete(right_tube);
    jackalope_node_delete(left_tube);
    jackalope_node_delete(system_audio);
    jackalope_node_delete(input_file);
    jackalope_graph_delete(graph);

    jackalope_shutdown();

    return 0;
}
