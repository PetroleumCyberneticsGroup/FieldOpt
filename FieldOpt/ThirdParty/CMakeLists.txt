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

option(BUILD_GP         "Build Gaussian Process library (gp)" ON )
option(BUILD_GTEST      "Build Google test framework"         ON )
option(BUILD_RPPP       "Build RpolyPlusPlus library"         ON )
option(BUILD_ECL        "Build ecl library from ERT"          ON )
option(BUILD_QT5        "Build Qt5 base libraries"            OFF)
option(USE_LOCAL_EIGEN3 "Use the local Eigen3 package"        ON )

message(STATUS "BUILD_GP:         ${BUILD_GP}" )
message(STATUS "BUILD_GTEST:      ${BUILD_GTEST}")
message(STATUS "BUILD_RPPP:       ${BUILD_RPPP}" )
message(STATUS "BUILD_ECL:        ${BUILD_ECL}")
message(STATUS "USE_LOCAL_EIGEN3: ${USE_LOCAL_EIGEN3}")
message(STATUS "BUILD_QT5: ${BUILD_QT5}")

message("*************************************************************")
message("Starting dependency builds ...")
message("*************************************************************")

if(USE_LOCAL_EIGEN3)
    set(EIGEN3_ROOT "${PROJECT_SOURCE_DIR}/Eigen3")
    message(STATUS "EIGEN3_ROOT: ${EIGEN3_ROOT}")
endif()

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

if(BUILD_QT5)
message("=============================================================")
message("Configuring Qt5 ...")
set(QT5_DIRECTORY "${PROJECT_SOURCE_DIR}/qtbase")
set(QT5_C_FLAG_PREFIX "-prefix")
set(QT5_C_PREFIX "${PROJECT_SOURCE_DIR}/libraries")
set(QT5_C_FLAG_XCB "-qt-xcb")
set(QT5_C_FLAG_REL "-release")
set(QT5_C_FLAG_OPS "-opensource")
set(QT5_C_FLAG_CLI "-confirm-license")
set(QT5_C_COMMAND "${QT5_DIRECTORY}/configure")
execute_process(COMMAND ${QT5_C_COMMAND}
                        ${QT5_C_FLAG_PREFIX} ${QT5_C_PREFIX}
                        ${QT5_C_FLAG_XCB} ${QT5_C_FLAG_REL}
                        ${QT5_C_FLAG_OPS} ${QT5_C_FLAG_CLI}
                WORKING_DIRECTORY ${QT5_DIRECTORY}
                RESULT_VARIABLE QT_C_RESULT
                OUTPUT_VARIABLE QT_C_OUTPUT
                #OUTPUT_QUIET
                )
message("Done configuring Qt5:" ${QT_C_RESULT})

message("Compiling Qt5 ...")
set(QT5_M_COMMAND "make")
execute_process(COMMAND ${QT5_M_COMMAND}
                WORKING_DIRECTORY ${QT5_DIRECTORY}
                RESULT_VARIABLE QT_M_RESULT
                OUTPUT_VARIABLE QT_M_OUTPUT)
message("Done compiling Qt5:" ${QT_M_RESULT})

message("Installing Qt5 ...")
set(QT5_I_COMMAND "make")
set(QT5_I_ARG "install")
execute_process(COMMAND ${QT5_I_COMMAND} ${QT5_I_ARG}
                WORKING_DIRECTORY ${QT5_DIRECTORY}
                RESULT_VARIABLE QT_I_RESULT
                OUTPUT_VARIABLE QT_I_OUTPUT)
message("Done installing Qt5:" ${QT_I_RESULT})
message("=============================================================")
endif()

message("\n*************************************************************")
message("Configuring done. If no errors occured, perform next steps:")
message("  1. make            to compile.")
message("  2. make install    to copy compiled files into libraries directory.")
message("  3. Enter the opm-common library and run cmake, make and make install")
message("     manually.")
message("*************************************************************")
