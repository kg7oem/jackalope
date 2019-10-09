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

add_library(
    jackalope SHARED

    jackalope/logging.cxx
    jackalope/log_dest.cxx
    jackalope/log_engine.cxx
    jackalope/thread.cxx
    jackalope/types.cxx
)
add_dependencies(jackalope jackalope-boost)
target_link_libraries(jackalope ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_subdirectory(tests/stage-1)

add_executable(jackalope-bin jackalope/jackalope.cxx)
target_link_libraries(jackalope-bin jackalope)
