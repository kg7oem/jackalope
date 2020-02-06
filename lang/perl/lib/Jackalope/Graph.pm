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

package Jackalope::Graph;

use strict;
use warnings;
use v5.10;

use Jackalope::Glue;
use Jackalope::Node;

use base qw(Jackalope::Object);

sub new {
    my ($class, @strings) = @_;
    my $graph = Jackalope::Glue::jackalope_graph_make(@strings);

    bless($graph, $class);

    return $graph;
}

sub make_node {
    my ($self, @strings) = @_;
    my $node = Jackalope::Glue::jackalope_graph_make_node($self, @strings);

    bless($node, 'Jackalope::Node');

    return $node;
}

*run = *Jackalope::Glue::jackalope_graph_run;

1;
