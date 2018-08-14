# - Try to find opm-common
# Once done, this will define
#
#  opm-common_FOUND - system has opm-common
#  opm-common_INCLUDE_DIRS - the opm-common include directories
#  opm-common_LIBRARIES - link these to use opm-common

include(LibFindMacros)

libfind_package(ecl ecl)

# Include dir
find_path(opm-common_INCLUDE_DIR
  NAMES opm
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/include
)

# Finally the library itself
find_library(opm-common_LIBRARY
  NAMES opmcommon
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
)

libfind_process(opm-common)


