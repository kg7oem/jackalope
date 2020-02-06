# begin configuration

set(JACKALOPE_VERSION_MAJOR 0)
set(JACKALOPE_VERSION_MINOR 0)
set(JACKALOPE_VERSION_PATCH 1)

# end of configuration

cmake_minimum_required(VERSION 3.8.2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

set(PROJECT jackalope)
project(${PROJECT})

set(JACKALOPE_VERSION ${JACKALOPE_VERSION_MAJOR}.${JACKALOPE_VERSION_MINOR}.${JACKALOPE_VERSION_PATCH})
add_definitions(-DJACKALOPE_VERSION=${JACKALOPE_VERSION})

set(JACKALOPE_BIN_TARGET ${PROJECT}-bin)
set(JACKALOPE_LIB_TARGET ${PROJECT}-lib)
set(JACKALOPE_SO_FILE ${PROJECT}.so)
set(JACKALOPE_LIBS -l${PROJECT})

include(ExternalProject)
include(CMake/LocalBoost.cmake)
include(InstallRequiredSystemLibraries)
include(CPackComponent)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "")

option(VERBOSE "Verbose builds" OFF)
option(DOWNLOAD_OK "Enable downloading of missing dependencies" OFF)
option(DOWNLOAD_BOOST_OK "Enable downloading of missing Boost" ${DOWNLOAD_OK})
option(FULL_BUILD "Enable testing for optional components by default" ON)
option(ENABLE_DBUS "Enable DBUS IPC" ${FULL_BUILD})
option(ENABLE_PERL "Enable Perl language support" ${FULL_BUILD})
option(ENABLE_JACKAUDIO "Enable JackAudio system support" ${FULL_BUILD})
option(ENABLE_LADSPA "Enable LADSPA audio plugin support" ${FULL_BUILD})
option(ENABLE_PORTAUDIO "Enable Portaudio support" ${FULL_BUILD})
option(ENABLE_RTAUDIO "Enable RtAudio support" ${FULL_BUILD})
option(ENABLE_SNDFILE "Enable libsndfile support" ${FULL_BUILD})
option(LOCAL_BOOST "Download and build a copy of Boost local to this project" OFF)

if (VERBOSE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif (VERBOSE)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-fPIC)
add_definitions(-Wfatal-errors -Werror)
add_definitions(-Wall -Wextra)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
#    add_definitions(-Og)
endif (${CMAKE_BUILD_TYPE} STREQUAL Debug)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# . gives us jackalope/ with out it being a system directory
include_directories(.)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

list(APPEND DEBIAN_DEV_PACKAGES build-essential cmake)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

add_library(
    ${JACKALOPE_LIB_TARGET} SHARED

    jackalope/async.cxx
    jackalope/audio.cxx
    jackalope/audio/gain.cxx
    jackalope/channel.cxx
    jackalope/foreign.cxx
    jackalope/graph.cxx
    jackalope/jackalope.cxx
    jackalope/log/dest.cxx
    jackalope/log/engine.cxx
    jackalope/message.cxx
    jackalope/network.cxx
    jackalope/node.cxx
    jackalope/object.cxx
    jackalope/plugin.cxx
    jackalope/property.cxx
    jackalope/signal.cxx
    jackalope/string.cxx
    jackalope/thread.cxx
    jackalope/types.cxx
)

if (LOCAL_BOOST)
    add_dependencies(${JACKALOPE_LIB_TARGET} jackalope-boost)
else()
    list(APPEND DEBIAN_DEV_PACKAGES libboost-date-time1.67-dev libboost-filesystem1.67-dev libboost-regex1.67-dev libboost-serialization1.67-dev libboost-system1.67-dev libboost-thread1.67-dev)
    list(APPEND DEBIAN_LIB_PACKAGES libboost-date-time1.67.0 libboost-filesystem1.67.0 libboost-regex1.67.0 libboost-serialization1.67.0 libboost-system1.67.0 libboost-thread1.67.0)
endif (LOCAL_BOOST)

target_link_libraries(${JACKALOPE_LIB_TARGET} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ldl)
set_target_properties(${JACKALOPE_LIB_TARGET} PROPERTIES OUTPUT_NAME ${PROJECT})

enable_testing()
add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope-bin.cxx)
target_link_libraries(jackalope-bin ${JACKALOPE_LIB_TARGET})
set_target_properties(jackalope-bin PROPERTIES OUTPUT_NAME "jackalope")

add_executable(jackalope-example-c examples/play_file.c)
target_link_libraries(jackalope-example-c ${JACKALOPE_LIB_TARGET})

add_executable(jackalope-example-c++ examples/play_file.cxx)
target_link_libraries(jackalope-example-c++ ${JACKALOPE_LIB_TARGET})

if(ENABLE_DBUS)
    message("Checking for dbus-c++")
    pkg_check_modules(DBUSLIB dbus-c++-1)

    list(APPEND DEBIAN_DEV_PACKAGES libdbus-1-dev libdbus-c++-dev)
    list(APPEND DEBIAN_LIB_PACKAGES libdbus-1-3 libdbus-c++-1-0v5)

    if (DBUSLIB_FOUND)
        message("  DBUS support is enabled")

        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/headers/dbus.adaptor.h
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_dbus_adaptor ${CMAKE_CURRENT_BINARY_DIR} jackalope/dbus.adaptor.xml ${CMAKE_CURRENT_BINARY_DIR}/headers/dbus.adaptor.h
            DEPENDS jackalope/dbus.adaptor.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        add_definitions(-DCONFIG_ENABLE_DBUS)
        target_compile_options(${JACKALOPE_LIB_TARGET} PUBLIC ${DBUSLIB_CFLAGS_OTHER})
        include_directories(SYSTEM ${DBUSLIB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/headers)
        target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/dbus.cxx ${CMAKE_CURRENT_BINARY_DIR}/headers/dbus.adaptor.h)
        target_link_libraries(${JACKALOPE_LIB_TARGET} ${DBUSLIB_LIBRARIES})
    endif (DBUSLIB_FOUND)
endif(ENABLE_DBUS)

if (ENABLE_PERL)
    message("Checking for Perl libraries")
    include(FindPerlLibs)

    list(APPEND DEBIAN_DEV_PACKAGES libperl-dev)
    list(APPEND DEBIAN_LIB_PACKAGES perl)

    if (PERLLIBS_FOUND)
        message("  Perl support is enabled")
        # FIXME get an out of tree build working
        set(PERL_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl)

        add_custom_target(
            jackalope-perl ALL
            DEPENDS ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
            DEPENDS ${JACKALOPE_LIB_TARGET}
        )

        add_custom_command(
            OUTPUT ${PERL_BUILD_DIR}/blib/arch/auto/Jackalope/Glue/Glue.so
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/Makefile.PL
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lang/perl/lib/Jackalope/Glue.xs
            COMMAND cd ${PERL_BUILD_DIR} && JACKALOPE_PERL_LD_FLAGS='-L${CMAKE_CURRENT_BINARY_DIR} ${JACKALOPE_LIBS}' ${PERL_EXECUTABLE} Makefile.PL INC=-I${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND cd ${PERL_BUILD_DIR} && make
        )

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PERL_BUILD_DIR}/blib)

    endif (PERLLIBS_FOUND)
endif (ENABLE_PERL)

if (ENABLE_JACKAUDIO)
    message("Checking for JACK audio")
    find_package(JACK)

    list(APPEND DEBIAN_DEV_PACKAGES libjack-jackd2-dev)
    list(APPEND DEBIAN_LIB_PACKAGES libjack-jackd2-0)

    if (JACK_FOUND)
        message("  JACK audio support is enabled")
        add_definitions(-DCONFIG_ENABLE_JACKAUDIO)
        include_directories(SYSTEM ${JACK_INCLUDE_DIR})
        target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/audio/jackaudio.cxx)
        target_link_libraries(${JACKALOPE_LIB_TARGET} ${JACK_LIBRARY})
    endif (JACK_FOUND)
endif (ENABLE_JACKAUDIO)

if (ENABLE_LADSPA)
    # there is no system libraries required for LADSPA
    message("LADSPA support is enabled")
    add_definitions(-DCONFIG_ENABLE_LADSPA)
    target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/audio/ladspa.cxx)
endif (ENABLE_LADSPA)

if (ENABLE_PORTAUDIO)
    message("Checking for Portaudio")
    find_package(Portaudio)

    list(APPEND DEBIAN_DEV_PACKAGES portaudio19-dev)
    list(APPEND DEBIAN_LIB_PACKAGES libportaudio2)

    if (PORTAUDIO_FOUND)
        message("  Portaudio support is enabled")
        add_definitions(-DCONFIG_ENABLE_PORTAUDIO)
        include_directories(SYSTEM ${PORTAUDIO_INCLUDE_DIR})
        target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/audio/portaudio.cxx)
        target_link_libraries(${JACKALOPE_LIB_TARGET} ${PORTAUDIO_LIBRARY})
    endif (PORTAUDIO_FOUND)
endif (ENABLE_PORTAUDIO)

if (ENABLE_RTAUDIO)
    message("Checking for RtAudio")
    find_package(RtAudio)

    list(APPEND DEBIAN_DEV_PACKAGES librtaudio-dev)
    list(APPEND DEBIAN_DEV_PACKAGES librtaudio6)

    if (RTAUDIO_FOUND)
        message("  librtaudio support is enabled")
        add_definitions(-DCONFIG_ENABLE_RTAUDIO)
        include_directories(SYSTEM ${RTAUDIO_INCLUDE_DIR})
        target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/audio/rtaudio.cxx)
        target_link_libraries(${JACKALOPE_LIB_TARGET} ${RTAUDIO_LIBRARY})
    endif (RTAUDIO_FOUND)
endif (ENABLE_RTAUDIO)

if (ENABLE_SNDFILE)
    message("Checking for libsndfile")
    find_package(Sndfile)

    list(APPEND DEBIAN_DEV_PACKAGES libsndfile1-dev)
    list(APPEND DEBIAN_DEV_PACKAGES libsndfile1)

    if (Sndfile_FOUND)
        message("  libsndfile support is enabled")
        add_definitions(-DCONFIG_ENABLE_SNDFILE)
        include_directories(SYSTEM ${Sndfile_INCLUDE_DIR})
        target_sources(${JACKALOPE_LIB_TARGET} PRIVATE jackalope/audio/sndfile.cxx)
        target_link_libraries(${JACKALOPE_LIB_TARGET} ${Sndfile_LIBRARY})
    endif (Sndfile_FOUND)

endif (ENABLE_SNDFILE)

add_custom_target(install-debian-packages apt install ${DEBIAN_DEV_PACKAGES} ${DEBIAN_LIB_PACKAGES})

install(
    TARGETS ${JACKALOPE_LIB_TARGET} ${JACKALOPE_BIN_TARGET}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY jackalope/
    DESTINATION include/jackalope/
    FILES_MATCHING PATTERN "*.h"
)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Jackalope Dataflow Engine")
set(CPACK_PACKAGE_VENDOR "Tyler Riddle KG7OEM")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/doc/license-LGPL3.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${JACKALOPE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${JACKALOPE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${JACKALOPE_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${DEBIAN_DEV_PACKAGES} ${DEBIAN_LIB_PACKAGES})

# must come after all the cpack configuration
INCLUDE(CPack)
