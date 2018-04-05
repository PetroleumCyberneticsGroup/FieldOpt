cmake_minimum_required(VERSION 2.8)
project(FieldOptThirdPartyDependencies)

########################################################################
# This CMake project builds all the third party dependencies needed by
# FieldOpt.
#
# You don't stricly need to build this. It is provided mainly as a
# convenience for building on systems where you don't have access (or
# permission to use) a package manager. If these libraries are not
# built, FieldOpt will use the libraries installed on the system.
#
# All the dependencies are found in this directory as git submodules.
# Note that these need to be initialized _before_ running the build.
#
# The libraries built here are:
#   - Qt5 (Core module)
#   - Boost (modules: program_options, serialization, mpi, system,
#                     filesystem, random)
#   - Eigen3
#   - RpolyPlusPlus
#   - libecl (ERT)
#   - libgp
#   - Google Test framework
########################################################################

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/libraries)
message("CMake install prefix: ${CMAKE_INSTALL_PREFIX}")

option(BUILD_GP    "Build Gaussian Process library (gp)" ON )
option(BUILD_GTEST "Build Google test framework"         ON)
option(BUILD_RPPP  "Build RpolyPlusPlus library"         ON )
option(BUILD_ECL   "Build ecl library from ERT"          OFF)

message(STATUS "BUILD_GP:     ${BUILD_GP}" )
message(STATUS "BUILD_GTEST:  ${BUILD_GTEST}")
message(STATUS "BUILD_RPPP:   ${BUILD_RPPP}" )
message(STATUS "BUILD_ECL:    ${BUILD_ECL}")

message("*************************************************************")
message("Starting dependency builds ...")
message("*************************************************************")

if(BUILD_GP)
message("=============================================================")
message("Configuring libgp ...")
add_subdirectory(libgp)
message("Done configuring libgp")
message("=============================================================")
endif()


if(BUILD_ECL)
message("=============================================================")
message("Configuring libecl ...")
add_subdirectory(libecl)
message("Done configuring libecl")
message("=============================================================")
endif()


if(BUILD_GTEST)
message("=============================================================")
message("Configuring gtest ...")
add_subdirectory(googletest)
message("Done configuring gtest")
message("=============================================================")
endif()


if(BUILD_RPPP)
message("=============================================================")
message("Configuring RpolyPlusPlus ...")
add_subdirectory(RpolyPlusPlus)
message("Done configuring RpolyPlusPlus")
message("=============================================================")
endif()

