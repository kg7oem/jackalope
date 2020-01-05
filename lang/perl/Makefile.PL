use 5.008005;
use ExtUtils::MakeMaker 7.12; # for XSMULTI option

WriteMakefile(
  NAME           => 'Jackalope',
  VERSION_FROM   => 'lib/Jackalope.pm',
  PREREQ_PM      => { 'ExtUtils::MakeMaker' => '7.12' },
  ABSTRACT_FROM  => 'lib/Jackalope.pm',
  AUTHOR         => 'Tyler Riddle KG7OEM',
  CCFLAGS        => '-Wall -std=c99',
  INC            => '-I../../',
  dynamic_lib    => {
    OTHERLDFLAGS => '-L../../build -ljackalope'
  },
  OPTIMIZE       => '-O3',
  LICENSE        => 'perl',
  XSMULTI        => 1,
);
