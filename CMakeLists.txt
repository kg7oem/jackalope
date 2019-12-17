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

add_definitions(-fPIC)
add_definitions(-Wfatal-errors -Werror)
add_definitions(-Wall -Wextra)

# . gives us jackalope/ with out it being a system directory
include_directories(.)

include(CMake/LocalBoost.cmake)

project(jackalope)

find_package(Threads REQUIRED)

# FIXME why doesn't this work?
# pkg_check_modules(LIBSNDFILE REQUIRED libsndfile-1)
# pkg_check_modules(PORTAUDIO REQUIRED libportaudio)

add_library(
    jackalope SHARED

    jackalope/channel.cxx
    jackalope/domain.cxx
    jackalope/driver.cxx
    jackalope/jackalope.cxx
    jackalope/log/dest.cxx
    jackalope/log/engine.cxx
#    jackalope/node.cxx
    jackalope/pcm.cxx
#    jackalope/pcm/ladspa.cxx
    jackalope/pcm/portaudio.cxx
#    jackalope/pcm/sndfile.cxx
    jackalope/property.cxx
#    jackalope/signal.cxx
    jackalope/string.cxx
    jackalope/thread.cxx
    jackalope/types.cxx
)
add_dependencies(jackalope jackalope-boost)
target_link_libraries(jackalope ${Boost_LIBRARIES})

target_link_libraries(jackalope ${CMAKE_THREAD_LIBS_INIT} -ldl -lsndfile -lportaudio)

enable_testing()
add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope-bin.cxx)
target_link_libraries(jackalope-bin jackalope)
