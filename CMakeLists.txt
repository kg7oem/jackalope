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
include(FindPythonLibs)

# . gives us jackalope/ with out it being a system directory
include_directories(.)
include_directories(SYSTEM ${PYTHON_INCLUDE_PATH})

# FIXME why doesn't this work?
# pkg_check_modules(LIBSNDFILE REQUIRED libsndfile-1)
# pkg_check_modules(PORTAUDIO REQUIRED libportaudio)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/swig.python.cxx ${CMAKE_BINARY_DIR}/jackalope.py
    DEPENDS ${CMAKE_SOURCE_DIR}/jackalope/swig.i
    COMMAND swig -c++ -python -o swig.python.cxx -outdir ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/jackalope/swig.i
)

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

add_library(
    jackalope_python SHARED

    ${CMAKE_BINARY_DIR}/swig.python.cxx
)
set_property(TARGET jackalope_python PROPERTY PREFIX "")
set_property(TARGET jackalope_python PROPERTY OUTPUT_NAME _jackalope)
target_link_libraries(jackalope_python jackalope ${PYTHON_LIBRARIES})

enable_testing()
add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope-bin.cxx)
target_link_libraries(jackalope-bin jackalope)
