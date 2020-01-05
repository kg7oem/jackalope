package Jackalope;

use strict;
use warnings;
use v5.10;

use Jackalope::Glue;
use Jackalope::Graph;
use Jackalope::Node;
use Jackalope::Object;

our $VERSION = 0.0.1;

*init = *Jackalope::Glue::jackalope_init;
*shutdown = *Jackalope::Glue::jackalope_shutdown;
