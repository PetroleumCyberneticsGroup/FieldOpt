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

message("*************************************************************")
message("Starting dependency builds ...")
message("*************************************************************")

message("=============================================================")
message("Building libgp ...")
add_subdirectory(libgp)
message("Done building libgp")
message("=============================================================")
