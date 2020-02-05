#!/usr/bin/env perl

# Jackalope Audio Engine
# Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

#This source code is licensed according to the Perl Artistic License 2.0 of
#which you can find a copy in the doc/ subdirectory of this project.

#This license establishes the terms under which a given free software Package
#may be copied, modified, distributed, and/or redistributed.The intent is that
#the Copyright Holder maintains some artistic control over the development of
#that Package while still keeping the Package available as open source and
#free software.

#You are always permitted to make arrangements wholly outside of this license
#directly with the Copyright Holder of a given Package. If the terms of this
#license do not permit the full use that you propose to make of the Package,
#you should contact the Copyright Holder and seek a different licensing
#arrangement.

use strict;
use warnings;
use v5.10;

use Jackalope;

use constant SAMPLE_RATE => 48000;
use constant BUFFER_SIZE => 512;
use constant LADSPA_ZAMTUBE_ID => 1515476290;

main();

sub main {
    my @files = @ARGV;
    die "must specify one or more files to play" unless @files;

    Jackalope::init;

    foreach my $file (@files) {
        play_file($file);
    }
}

sub play_file {
    my ($filename) = @_;

    die "not a file: $filename" unless -f $filename;

    my $graph = Jackalope::Graph->new(
        "pcm.sample_rate" => SAMPLE_RATE,
        "pcm.buffer_size" => BUFFER_SIZE,
    );

    my $input_file = $graph->add_node(
        "object.type" => "audio::sndfile",
        "node.name" => "input file",
        "config.path", $filename,
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

    $input_file->subscribe("object.stopped", $graph, "object.stop");

    if ($input_file->get_num_sources == 1) {
        $input_file->link("Output 1", $left_tube, "Audio Input 1");
        $input_file->link("Output 1", $right_tube, "Audio Input 1");
    } else {
        $input_file->link("Output 1", $left_tube, "Audio Input 1");
        $input_file->link("Output 2", $right_tube, "Audio Input 1");
    }

    $left_tube->link("Audio Output 1", $system_audio, "left");
    $right_tube->link("Audio Output 1", $system_audio, "right");

    $graph->run;

    return;
}
