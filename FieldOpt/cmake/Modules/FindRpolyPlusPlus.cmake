# - Try to find RpolyPlusPlus
# Once done, this will define
#
#  RpolyPlusPlus_FOUND - system has RpolyPlusPlus
#  RpolyPlusPlus_INCLUDE_DIRS - the RpolyPlusPlus include directories
#  RpolyPlusPlus_LIBRARIES - link these to use RpolyPlusPlus

include(LibFindMacros)

# Dependencies
libfind_package(Eigen3 Eigen3)


## Use pkg-config to get hints about paths
##libfind_pkg_check_modules(Magick++_PKGCONF ImageMagick++)

# Include dir
find_path(RpolyPlusPlus_INCLUDE_DIR
  NAMES polynomial.h find_polynomial_roots_jenkins_traub.h
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/include/RpolyPlusPlus
)

# Finally the library itself
find_library(RpolyPlusPlus_LIBRARY
  NAMES rpoly_plus_plus
  PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
)

libfind_process(RpolyPlusPlus)


