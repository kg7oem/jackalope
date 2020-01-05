cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ExternalProject)

set(CMAKE_BUILD_TYPE Debug)

option(VERBOSE "Verbose builds" OFF)

if (VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif (VERBOSE)

# don't change anything below this line
set(CMAKE_CXX_STANDARD 17)

include(CMake/LocalBoost.cmake)

add_definitions(-fPIC)
add_definitions(-Wfatal-errors -Werror)
add_definitions(-Wall -Wextra)

add_definitions(-Og)

# . gives us jackalope/ with out it being a system directory
include_directories(.)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

project(jackalope)

find_package(Threads REQUIRED)

# . gives us jackalope/ with out it being a system directory
include_directories(.)

# FIXME why doesn't this work?
# pkg_check_modules(LIBSNDFILE REQUIRED libsndfile-1)
# pkg_check_modules(PORTAUDIO REQUIRED libportaudio)

add_library(
    jackalope SHARED

    jackalope/async.cxx
    jackalope/audio.cxx
    jackalope/audio/ladspa.cxx
    jackalope/audio/portaudio.cxx
    jackalope/audio/sndfile.cxx
    jackalope/channel.cxx
    jackalope/foreign.cxx
    jackalope/graph.cxx
    jackalope/jackalope.cxx
    jackalope/log/dest.cxx
    jackalope/log/engine.cxx
    jackalope/message.cxx
    jackalope/node.cxx
    jackalope/object.cxx
    jackalope/property.cxx
    jackalope/signal.cxx
    jackalope/string.cxx
    jackalope/thread.cxx
    jackalope/types.cxx
)
add_dependencies(jackalope jackalope-boost)
target_link_libraries(jackalope ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ldl)
target_link_libraries(jackalope -lsndfile -lportaudio)

enable_testing()
add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope-bin.cxx)
target_link_libraries(jackalope-bin jackalope)

include(FindPerlLibs)

if (PERLLIBS_FOUND)

set(PERL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/jackalope-perl)

add_custom_target(
    jackalope-perl ALL
    DEPENDS ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
    DEPENDS jackalope
)

add_custom_command(
    OUTPUT ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/Makefile.PL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/lib/Jackalope/Glue.xs
    COMMAND mkdir -p ${PERL_BUILD_DIR}
    COMMAND cp -pr ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/* ${PERL_BUILD_DIR}
    COMMAND cd ${PERL_BUILD_DIR} && JACKALOPE_PERL_LD_FLAGS='-L${CMAKE_CURRENT_BINARY_DIR} -ljackalope' ${PERL_EXECUTABLE} Makefile.PL INC=-I${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND cd ${PERL_BUILD_DIR} && make
)

endif (PERLLIBS_FOUND)
