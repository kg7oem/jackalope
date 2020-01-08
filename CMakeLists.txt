cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
include(ExternalProject)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE Debug)
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "")

option(VERBOSE "Verbose builds" OFF)
option(FULL_BUILD "Enable testing for optional components by default" ON)
option(ENABLE_DBUS "Enable DBUS IPC" ${FULL_BUILD})
option(ENABLE_PERL "Enable Perl language support" ${FULL_BUILD})
option(ENABLE_JACKAUDIO "Enable JackAudio system support" ${FULL_BUILD})
option(ENABLE_LADSPA "Enable LADSPA audio plugin support" ${FULL_BUILD})
option(ENABLE_PORTAUDIO "Enable Portaudio support" ${FULL_BUILD})
option(ENABLE_SNDFILE "Enable libsndfile support" ${FULL_BUILD})

if (VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif (VERBOSE)

# don't change anything below this line
set(CMAKE_CXX_STANDARD 17)

include(CMake/LocalBoost.cmake)

add_definitions(-fPIC)
add_definitions(-Wfatal-errors -Werror)
add_definitions(-Wall -Wextra)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
add_definitions(-Og)
endif (${CMAKE_BUILD_TYPE} STREQUAL Debug)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# . gives us jackalope/ with out it being a system directory
include_directories(.)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

project(jackalope)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

add_library(
    jackalope SHARED

    jackalope/async.cxx
    jackalope/audio.cxx
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
# FIXME broken by dbus
# add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope-bin.cxx)
target_link_libraries(jackalope-bin jackalope)

add_executable(jackalope-example-c examples/play_file.c)
target_link_libraries(jackalope-example-c jackalope)

add_executable(jackalope-example-c++ examples/play_file.cxx)
target_link_libraries(jackalope-example-c++ jackalope)

if(ENABLE_DBUS)
    message("Checking for dbus-c++")
    pkg_check_modules(DBUSLIB dbus-c++-1)

    if (DBUSLIB_FOUND)
        message("  DBUS support is enabled")
        add_custom_command(
            OUTPUT jackalope/dbus.adaptor.h
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_dbus_adaptor ${CMAKE_CURRENT_BINARY_DIR} dbus.adaptor.xml dbus.adaptor.h
            DEPENDS jackalope/dbus.adaptor.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/jackalope
        )

        add_definitions(-DCONFIG_ENABLE_DBUS)
        target_compile_options(jackalope PUBLIC ${DBUSLIB_CFLAGS_OTHER})
        include_directories(SYSTEM ${DBUSLIB_INCLUDE_DIRS})
        target_sources(jackalope PRIVATE jackalope/dbus.cxx jackalope/dbus.adaptor.h )
        target_link_libraries(jackalope ${DBUSLIB_LIBRARIES})
    endif (DBUSLIB_FOUND)
endif(ENABLE_DBUS)

if (ENABLE_PERL)
    message("Checking for Perl libraries")
    include(FindPerlLibs)

    if (PERLLIBS_FOUND)
        message("  Perl support is enabled")
        # FIXME get an out of tree build working
        set(PERL_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl)

        add_custom_target(
            jackalope-perl ALL
            DEPENDS ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
            DEPENDS jackalope
        )

        add_custom_command(
            OUTPUT ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/Makefile.PL
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/lib/Jackalope/Glue.xs
            COMMAND cd ${PERL_BUILD_DIR} && JACKALOPE_PERL_LD_FLAGS='-L${CMAKE_CURRENT_BINARY_DIR} -ljackalope' ${PERL_EXECUTABLE} Makefile.PL INC=-I${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND cd ${PERL_BUILD_DIR} && make
        )

    endif (PERLLIBS_FOUND)
endif (ENABLE_PERL)

if (ENABLE_JACKAUDIO)
    message("Checking for JACK audio")
    find_package(JACK)

    if (JACK_FOUND)
        message("  JACK audio support is enabled")
        add_definitions(-DCONFIG_ENABLE_JACKAUDIO)
        include_directories(SYSTEM ${JACK_INCLUDE_DIR})
        target_sources(jackalope PRIVATE jackalope/audio/jackaudio.cxx)
        target_link_libraries(jackalope ${JACK_LIBRARY})
    endif (JACK_FOUND)
endif (ENABLE_JACKAUDIO)

if (ENABLE_LADSPA)
    # there is no system libraries required for LADSPA
    message("LADSPA support is enabled")
    add_definitions(-DCONFIG_ENABLE_LADSPA)
    target_sources(jackalope PRIVATE jackalope/audio/ladspa.cxx)
endif (ENABLE_LADSPA)

if (ENABLE_PORTAUDIO)
    message("Checking for Portaudio")
    find_package(Portaudio)

    if (PORTAUDIO_FOUND)
        message("  Portaudio support is enabled")
        add_definitions(-DCONFIG_ENABLE_PORTAUDIO)
        include_directories(SYSTEM ${PORTAUDIO_INCLUDE_DIR})
        target_sources(jackalope PRIVATE jackalope/audio/portaudio.cxx)
        target_link_libraries(jackalope ${PORTAUDIO_LIBRARY})
    endif (PORTAUDIO_FOUND)
endif (ENABLE_PORTAUDIO)

if (ENABLE_SNDFILE)
    message("Checking for libsndfile")
    find_package(Sndfile)

    if (Sndfile_FOUND)
        message("  libsndfile support is enabled")
        add_definitions(-DCONFIG_ENABLE_SNDFILE)
        include_directories(SYSTEM ${Sndfile_INCLUDE_DIR})
        target_sources(jackalope PRIVATE jackalope/audio/sndfile.cxx)
        target_link_libraries(jackalope ${Sndfile_LIBRARY})
    endif (Sndfile_FOUND)

endif (ENABLE_SNDFILE)
