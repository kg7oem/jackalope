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

use 5.008005;
use ExtUtils::MakeMaker 7.12; # for XSMULTI option

WriteMakefile(
  NAME           => 'Jackalope',
  VERSION_FROM   => 'lib/Jackalope.pm',
  PREREQ_PM      => { 'ExtUtils::MakeMaker' => '7.12' },
  ABSTRACT_FROM  => 'lib/Jackalope.pm',
  AUTHOR         => 'Tyler Riddle KG7OEM',
  CCFLAGS        => '-Wall -std=c99',
  dynamic_lib    => {
    # FIXME how is this passed in via the command line?
    OTHERLDFLAGS => $ENV{JACKALOPE_PERL_LD_FLAGS},
  },
  OPTIMIZE       => '-O3',
  LICENSE        => 'perl',
  XSMULTI        => 1,
);
