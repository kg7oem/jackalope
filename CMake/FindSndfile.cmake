# - Try to find libsndfile include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(Sndfile)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  Sndfile_ROOT_DIR         Set this variable to the root installation of
#                            libsndfile if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  Sndfile_FOUND            System has libsndfile, include and lib dirs found
#  Sndfile_INCLUDE_DIR      The libsndfile include directories.
#  Sndfile_LIBRARY          The libsndfile library.

find_path(Sndfile_ROOT_DIR
    NAMES include/sndfile.h
)

find_path(Sndfile_INCLUDE_DIR
    NAMES sndfile.h
    HINTS ${Sndfile_ROOT_DIR}/include
)

find_library(Sndfile_LIBRARY
    NAMES sndfile
    HINTS ${Sndfile_ROOT_DIR}/lib
)

if(Sndfile_INCLUDE_DIR AND Sndfile_LIBRARY)
  set(Sndfile_FOUND TRUE)
else(Sndfile_INCLUDE_DIR AND Sndfile_LIBRARY)
  FIND_LIBRARY(Sndfile_LIBRARY NAMES Sndfile)
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sndfile DEFAULT_MSG Sndfile_INCLUDE_DIR Sndfile_LIBRARY )
  MARK_AS_ADVANCED(Sndfile_INCLUDE_DIR Sndfile_LIBRARY)
endif(Sndfile_INCLUDE_DIR AND Sndfile_LIBRARY)

mark_as_advanced(
    Sndfile_ROOT_DIR
    Sndfile_INCLUDE_DIR
    Sndfile_LIBRARY
)
