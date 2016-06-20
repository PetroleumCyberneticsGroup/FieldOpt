cmake_minimum_required(VERSION 3.2)
project(constraintmath LANGUAGES CXX)

find_library(RpolyPlusPlus rpoly_plus_plus REQUIRED)

add_library(constraintmath
        well_constraint_projections/well_constraint_projections.cpp
        )
add_library(fieldopt::constraintmath ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries(constraintmath
        PUBLIC Qt5::Core
        PUBLIC ${RpolyPlusPlus}
        PUBLIC fieldopt::ertwrapper
        PUBLIC fieldopt::reservoir
        PUBLIC fieldopt::utilities)

target_include_directories(constraintmath PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/well_constraint_projections>)

target_compile_features(constraintmath
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
add_executable(test_constraintmath
        tests/well_constraint_projections_tests.cpp
        tests/test_domain_boundary.cpp
        )
target_link_libraries(test_constraintmath
        fieldopt::constraintmath
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_constraintmath COMMAND $<TARGET_FILE:test_constraintmath>)
