cmake_minimum_required(VERSION 2.8)
project(constraintmath)

# Dbg ======================================================
message("\n================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Find libraries ===========================================
#find_library(RpolyPlusPlus
#        NAMES rpoly_plus_plus
#        HINTS ${CMAKE_BINARY_DIR}/libraries/lib
#        REQUIRED)
#include_directories(${CMAKE_BINARY_DIR}/libraries/include/)

# Add library ==============================================
add_library(constraintmath
    well_constraint_projections/well_constraint_projections.cpp)

add_library(fieldopt::constraintmath ALIAS ${PROJECT_NAME})

# Target link libraries ====================================
target_link_libraries(constraintmath
    PUBLIC Qt5::Core
    PUBLIC ${RpolyPlusPlus_LIBRARIES}
    PUBLIC fieldopt::ertwrapper
    PUBLIC fieldopt::reservoir)

target_include_directories(constraintmath PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/well_constraint_projections>)

# Compile options ==========================================
add_compile_options(-std=c++11)

# Unit tests ===============================================
if (BUILD_TESTING)
  # Unit tests
  include_directories(${EIGEN3_INCLUDE_DIR})

  add_executable(test_constraintmath
      tests/well_constraint_projections_tests.cpp
      tests/test_domain_boundary.cpp
      )

  target_link_libraries(test_constraintmath
      fieldopt::constraintmath
      ${gtest}
      ${gtest_main}
      ${CMAKE_THREAD_LIBS_INIT})

  add_test(NAME test_constraintmath
      COMMAND $<TARGET_FILE:test_constraintmath>)
endif()

# Install targets ==========================================
install(TARGETS constraintmath
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

# Debug: include dirs ======================================
include(../InclDirDbg.cmake)