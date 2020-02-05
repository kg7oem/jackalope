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

use File::Basename qw(basename);

use Jackalope;

use constant LADSPA_ZAMTUBE_ID => 1515476290;

Jackalope::init;

my $graph = Jackalope::Graph->new(
    "pcm.sample_rate" => 48000,
    "pcm.buffer_size" => 256,
);

my $system_audio = $graph->make_node(
    "object.type" => "audio::jackaudio",
    "node.name" => "system audio",
    "config.client_name", basename($0),
    "source.left output" => "audio",
    "source.right output" => "audio",
    "sink.left input" => "audio",
    "sink.right input" => "audio",
);

my $left_tube = $graph->make_node(
    "object.type" => "audio::ladspa",
    "node.name" => "left tube",
    "plugin.id" => LADSPA_ZAMTUBE_ID,
);

my $right_tube = $graph->make_node(
    "object.type" => "audio::ladspa",
    "node.name" => "right tube",
    "plugin.id" => LADSPA_ZAMTUBE_ID,
);

$system_audio->link("left output", $left_tube, "Audio Input 1");
$system_audio->link("right output", $right_tube, "Audio Input 1");
$left_tube->link("Audio Output 1", $system_audio, "left input");
$right_tube->link("Audio Output 1", $system_audio, "right input");

$graph->run;
