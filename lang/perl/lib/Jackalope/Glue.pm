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
