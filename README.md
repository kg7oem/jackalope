Jackalope Dataflow Engine

Jackalope is a work in progress replacement for the Pulsar Audio Engine proof
of concept. The core dataflow engine is similiar in spirit and is inspired by
GNU Radio. The initial use case for the engine is to create a live broadcast
audio control system for the Internet and radio transmission. It is expected
that the engine will also work well with MIDI, image, and video data as well
as other types of signals and data transformation tasks.

Project Status

This is currently bleeding edge and highly volitile source code. Things can
and will change. Stuff may not work. It might crash. In no way should this
project be considered production quality.

Objects

Jackalope objects encapualate components that accept input, generate output,
or both. The output of an object is associated with the inputs of other
objects. An object input is available when it has no associations or
when all the associated outputs have data available. When all of the
required inputs are available for an object it will be executed.

Object inputs and outputs are named and take the form of:

  * Control and state variables which are strongly typed
  * Signals and slots which contain no associated data
  * Sources and sinks which have associated strongly typed data

Objects have zero or more properties associated with them. Each property is
named, has a strong type, and can be read-write or read-only. Properties are
used to configure the objects and query their status. Object control and state
variables are exposed as properties.

Some examples of objects are:

  * Volume adjusting audio effect with one floating point PCM source and sink,
    a single floating point configuration variable for the volume adjustment
    value, and one floating point state variable for the current output level.

  * Volume muting audio effect with a single source and sink and a single
    boolean configuration property that selects if the audio is muted or
    not muted.

  * Audio driver allowing reading and writing to the system audio devices and
    with signals for indicating buffer underflow and overflow conditions.

  * Audio file playback tool with one or more sources for the audio output,
    commands to start, stop and pause playback, signals to indicate when
    playback is started, stopped, and paused, and state properties indicating
    the current playback position.

Sources and Sinks

The sources and sinks for an object share seperate namespaces and their order
is significant. Ordering and data types will be used when automatically forming
associations between objects. Sources and sinks are each types of channels and
a link encapsulates a connection from a source to a sink.

Nodes and Graphs

Nodes are the most common objects that users will encounter. A node is a type
of object that is owned by a graph which is also a type of object. All nodes
must have a parent graph and nodes must only form associations with objects
in the same graph.

Graphs can have their own sources and sinks which are associated with the
sources and sinks of objects the graph owns. This concept is extended to
properties of the graph which allows a graph to be a collection of objects
with a public interface.

Audio Processing Systems

An audio signal processing system is built with Jackalope by defining a
dataflow graph made out of objects encapsulating the desired audio effects.
This example uses the JACK Audio Connection Kit to adjust the volume of an
audio signal from a microphone and provide a new audio signal that is the
processed result:

    using namespace jackalope::foreign;

    auto gain_changer = make_graph({
      { "pcm.sample rate", 48000 },
      { "pcm.buffer size", 128 },
    });

    auto jackaudio = gain_changer.add_object({
        { "object.class", "driver::jackaudio" },
        { "object.name", "system audio" },
        { "source.microphone", "pcm" },
        { "sink.voice", "pcm" },
    });

    auto gain = gain_changer.add_object({
        { "object.class", "audio::gain" },
        { "object.name", "input gain" },
        // add 12db of gain
        { "control.gain", 12 },
    });

    jackaudio.link("microphone", volume, "main input");
    gain.link("main output", jackaudio, "voice");

    // returns when graph stops
    gain_changer.run();

Audio Control Systems

Preparing a voice signal for broadcast is not that simple though. The bare
minimum processing required is dynamic range reduction and some light
equalization. A cough button is also a good idea if the application is
live broadcast. Building such a graph looks like this:

    using namespace jackalope::foreign;

    auto voice_processor = make_graph({
      { "pcm.sample rate", 48000 },
      { "pcm.buffer size", 128 },
    });

    auto jackaudio = voice_processor.add_object({
        { "object.class", "driver::jackaudio" },
        { "object.name", "system audio" },
        { "source.microphone", "pcm" },
        { "sink.voice", "pcm" },
    });

    auto cough = voice_processor.add_object({
      { "object.class", "audio::mute" },
      { "object.name", "cough" },
      { "control.muted", false },
    });

    auto input_gain = voice_processor.add_object({
        { "object.class", "audio::gain" },
        { "object.name", "input gain" },
        { "control.gain", 12 },
    });

    auto output_gain = voice_processor.add_object({
        { "object.class", "audio::gain" },
        { "object.name", "output gain" },
        { "control.gain", 12 },
    });

    auto comp1 = voice_processor.add_object({
      { "object.class", "audio::dynamic range compressor" },
      { "object.name", "compressor 1" },
      { "control.attack", 1 },
      { "control.release", 2000 },
      { "control.threshold", -30 },
      { "control.ratio", 8 },
      { "control.knee", 4 },
      { "control.makeup", 14 }
    });

    auto comp2 = voice_processor.add_object({
      { "object.class", "audio::dynamic range compressor" },
      { "object.name", "compressor 1" },
      { "control.attack", 0.1 },
      { "control.release", 500 },
      { "control.threshold", -26 },
      { "control.ratio", 2 },
      { "control.knee", 4 },
      { "control.makeup", 20 }
    });

    auto equalizer = voice_processor.add_object({
      { "control.center", 630 },
      { "control.bandwidth", 3 },
      { "control.gain", -8 },
    });

    jackaudio.link("microphone", input_gain, "main input");
    input_gain.link("main output", cough, "main input");
    cough.link("main output", comp1, "main input");
    comp1.link("main output", comp2, "main input");
    comp2.link("main output", equalizer, "main input");
    equalizer.link("main output", output_gain, "main input");
    output_gain.link("main output", jackaudio, "voice");

    // executes in other threads and returns control
    // immediately
    voice_processor.start();

    while(true) {
      bool should_mute = read_cough_button();
      cough.set("control.muted", should_mute);

      if (! voice_processor->running) {
        break;
      }

      // refresh around 50hz
      std::this_thread::sleep_for(1 / 50);
    }

IPC

Jackalope is intended to be incorporated into existing software or function
as a standalone program using a configuration file and optional interprocess
communication. Language bindings will be available in C, C++, and high level
languages such as Python and Perl.

When running as a standalone program the configuration file defines the
dataflow graph. If any other logic is required that is defined in another
program and the Jackalope objects are controlled using interprocess
communication. The graph from the audio control system example looks like this
as a YAML file:

    #!/usr/bin/env jackalope

    properties:
      - IPC.name: example

    defaults:
      - pcm.sample rate: 48000
      - pcm.buffer size: 256

    objects:
      system audio:
        properties:
          - object.class: jackaudio
          - source.microphone: pcm
          - sink.voice: pcm
        link:
          microphone: input gain.main input

      input gain:
        properties:
          - object.class: audio::gain
          - control.gain: 12
        link:
          main output: cough.main input

      cough:
        properties:
          - object.class: audio::mute
          - control.muted: false
        link:
          main output: comp 1.main input

      comp 1:
        properties:
          - object.class: audio::dynamic range compressor
          - control.attack: 1
          - control.release: 2000
          - control.threshold: -30
          - control.ratio: 8
          - control.knee: 4
          - control.makeup: 14
        link:
          main output: comp 2.main input

      comp 2:
        properties:
          - object.class: audio::dynamic range compressor
          - control.attack: 0.1
          - control.release: 500
          - control.threshold: -26
          - control.ratio: 2
          - control.knee: 4
          - control.makeup: 20
        link:
          main output: equalizer.main input

      equalizer:
        properties:
          - object.class: audio::equalizer
          - control.center: 630
          - control.bandwidth: 3
          - control.gain: -8
        link:
          main output: output gain.main input

      output gain:
        properties:
          - object.class: audio::gain
          - control.gain: 12
        link:
          main output: system audio.voice

Once the standalone Jackalope program is running with this graph another
standalone program handles updating the cough button mute status.

    #!/usr/bin/env perl

    use strict;
    use warnings;
    use Jackalope::IPC;

    my $jackalope = Jackalope::IPC->open("example");
    my $cough = $jackalope->get_object("cough");

    while(1) {
      my $should_mute = read_cough_button();
      cough->set("control.muted", $should_mute);
      // refresh around 50hz
      sleep(1 / 50);
    }


C++ Class Hierarchy

  * jackalope::object_t (commands, messages, properties, signals, slots)

    * jackalope::graph_t (node container)

    * jackalope::node_t (sources, sinks)

      * jackalope::network_t (reusable component)

        * jackalope::linear_network_t

      * jackalope::plugin_t (execution)

        * jackalope::filter_plugin_t (execute when all sources are ready)

        * jackalope::petri_plugin_t (execution conditions)


Class jackalope::object_t

  * lifecycle methods: init(), start(), stop()

  * property methods: peek(), poke()

  * message methods: add_message_handler(), send_message(), deliver_messages(), deliver_one_message(), deliver_if_needed()

  * signal methods: connect()

Class jackalope::node_t

  * lifecycle methods: activate()

Class jackalope::filter_plugin_t

  * plugin methods: execute()

Class jackalope::petri_plugin_t

  * plugin methods: add_marking()
