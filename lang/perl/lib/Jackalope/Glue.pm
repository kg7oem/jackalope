package Jackalope::Glue;

use strict;
use warnings;
use v5.10;

use XSLoader;

XSLoader::load();

# pack perl strings into something that can be
# cast to a char ** for C
sub pack_strings {
    my $ptrs = pack("P" x @_, @_);
    $ptrs .= pack("LL", 0, 0);
}

sub jackalope_graph_make {
    my $packed = pack_strings(@_);

    return _jackalope_graph_make($packed);
}

sub jackalope_graph_add_node {
    my ($graph, @strings) = @_;
    my $packed = pack_strings(@strings);

    return _jackalope_graph_add_node($graph, $packed);
}

1;
