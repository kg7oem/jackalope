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

sub add_node {
    my ($self, @strings) = @_;
    my $node = Jackalope::Glue::jackalope_graph_add_node($self, @strings);

    bless($node, 'Jackalope::Node');

    return $node;
}

*run = *Jackalope::Glue::jackalope_graph_run;

1;
