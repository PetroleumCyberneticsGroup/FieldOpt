# - Try to find ecl
# Once done, this will define
#
#  ecl_FOUND - system has ecl
#  ecl_INCLUDE_DIRS - the ecl include directories
#  ecl_LIBRARIES - link these to use ecl

include(LibFindMacros)

# Include dir
find_path(ecl_INCLUDE_DIR
  NAMES ert
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/include
)

# Finally the library itself
find_library(ecl_LIBRARY
  NAMES ecl
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
)

libfind_process(ecl)


