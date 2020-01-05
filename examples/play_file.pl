#!/usr/bin/env perl

use strict;
use warnings;
use v5.10;

use Jackalope;

use constant LADSPA_ZAMTUBE_ID => 1515476290;

unless (defined $ARGV[0]) {
    die "must specify a file to play";
}

Jackalope::init;

my $graph = Jackalope::Graph->new(
    "pcm.sample_rate" => 48000,
    "pcm.buffer_size" => 512,
);

my $input_file = $graph->add_node(
    "object.type" => "audio::sndfile",
    "node.name" => "input file",
    "config.path", $ARGV[0],
);

my $system_audio = $graph->add_node(
    "object.type" => "audio::portaudio",
    "node.name" => "system audio",
    "sink.left" => "audio",
    "sink.right" => "audio",
);

my $left_tube = $graph->add_node(
    "object.type" => "audio::ladspa",
    "node.name" => "left tube",
    "plugin.id" => LADSPA_ZAMTUBE_ID,
);

my $right_tube = $graph->add_node(
    "object.type" => "audio::ladspa",
    "node.name" => "right tube",
    "plugin.id" => LADSPA_ZAMTUBE_ID,
);

$input_file->connect("object.stopped", $graph, "object.stop");
$input_file->link("Output 1", $left_tube, "Audio Input 1");
$input_file->link("Output 1", $right_tube, "Audio Input 1");
$left_tube->link("Audio Output 1", $system_audio, "left");
$right_tube->link("Audio Output 1", $system_audio, "right");

$graph->run;

Jackalope::shutdown();
