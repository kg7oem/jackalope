# - Try to find RtAudio include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(RtAudio)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  RTAUDIO_ROOT_DIR         Set this variable to the root installation of
#                            RtAudio if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  RTAUDIO_FOUND            System has RtAudio, include and lib dirs found
#  RTAUDIO_INCLUDE_DIR      The RtAudio include directories.
#  RTAUDIO_LIBRARY          The RtAudio library.

find_path(RTAUDIO_ROOT_DIR
    NAMES include/rtaudio/RtAudio.h
)

find_path(RTAUDIO_INCLUDE_DIR
    NAMES rtaudio/RtAudio.h
    HINTS ${RTAUDIO_ROOT_DIR}/include
)

find_library(RTAUDIO_LIBRARY
    NAMES rtaudio
    HINTS ${RTAUDIO_ROOT_DIR}/lib
)

if(RTAUDIO_INCLUDE_DIR AND RTAUDIO_LIBRARY)
  set(RTAUDIO_FOUND TRUE)
else(RTAUDIO_INCLUDE_DIR AND RTAUDIO_LIBRARY)
  FIND_LIBRARY(RTAUDIO_LIBRARY NAMES RTAUDIO)
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTAUDIO DEFAULT_MSG RTAUDIO_INCLUDE_DIR RTAUDIO_LIBRARY )
  MARK_AS_ADVANCED(RTAUDIO_INCLUDE_DIR RTAUDIO_LIBRARY)
endif(RTAUDIO_INCLUDE_DIR AND RTAUDIO_LIBRARY)

mark_as_advanced(
    RTAUDIO_ROOT_DIR
    RTAUDIO_INCLUDE_DIR
    RTAUDIO_LIBRARY
)
