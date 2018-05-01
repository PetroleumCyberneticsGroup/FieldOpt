cmake_minimum_required(VERSION 2.8)
project(ertwrapper)

# Dbg ==================================================================
message("\n============================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Options ==============================================================


# Find libraries =======================================================
# ERT (
# lib_ecl.so linked by ERTWrapper and WIC
# ERTWrapper lib linked into Reservoir
# Reservoir linked into Model
# Model linked into Simulation
# Runner linking ERTWrapper, Model, Simulation, Settings,
# Optimization, hdf5summaryreader, constraintmath

message("\n*************************************************")
message("--------> Set ERTLIB: ${ERTLIB} <--------")
message("*************************************************")

# Add library ==========================================================
add_library(ertwrapper
    ertwrapper_exceptions.h
    eclgridreader.cpp
    eclsummaryreader.cpp)

add_library(fieldopt::ertwrapper
    ALIAS ${PROJECT_NAME})

# Include dirs =========================================================
#include_directories(
#        ${EIGEN3_INCLUDE_DIR}
#        ${ERT_INCLUDE_DIR})

# Target link libraries ================================================
#if (ERT_STANDARD)
#  target_link_libraries(ertwrapper
#      PUBLIC ${ERTLIB}
#      PUBLIC ${ert_util}
#      )
#
#else()
  target_link_libraries(ertwrapper
      # PUBLIC ${CMAKE_SOURCE_DIR}/${ERTLIB}
      PUBLIC ${ERTLIB}
      PUBLIC ${ert_util}
      )
#endif ()

# Target include directories ===========================================
target_include_directories(ertwrapper PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Compile options ======================================================
add_compile_options(-std=c++11)

# Enable position independent code when compiling lib, so that
# shared library libwellindexcalculator.so may be linked with
# static reservoir and ertwrapper libraries
if (BUILD_WIC_ADGPRS)
  set_target_properties(ertwrapper
      PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

# Unit tests ===========================================================
if (BUILD_TESTING)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_executable(test_ertwrapper
      tests/test_eclgridreader.cpp
      tests/test_eclsummaryreader.cpp
      )
  target_link_libraries(test_ertwrapper
      fieldopt::ertwrapper
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT})

  add_test(NAME test_ertwrapper
      COMMAND $<TARGET_FILE:test_ertwrapper>)
endif()

# Install targets ======================================================
install(TARGETS ertwrapper
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

# Debug: include dirs ==================================================
include(../InclDirDbg.cmake)