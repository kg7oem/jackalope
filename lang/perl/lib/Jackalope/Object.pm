package Jackalope::Object;

use strict;
use warnings;
use v5.10;

use Jackalope::Glue;

*DESTROY = *Jackalope::Glue::jackalope_object_delete;
*connect = *Jackalope::Glue::jackalope_object_connect;
*link = *Jackalope::Glue::jackalope_object_link;
*start = *Jackalope::Glue::jackalope_object_start;
*stop = *Jackalope::Glue::jackalope_object_stop;

1;
