cmake_minimum_required(VERSION 3.2)
project(wellindexcalculator LANGUAGES CXX)

find_library(RpolyPlusPlus rpoly_plus_plus REQUIRED)

add_library(wellindexcalculator
        geometry_functions/geometryfunctions_exceptions.h
        geometry_functions/geometryfunctions.cpp
        well_constraint_projections/well_constraint_projections.cpp
        wellindexcalculator.cpp)
add_library(fieldopt::wellindexcalculator ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries(wellindexcalculator
        PUBLIC Qt5::Core
        PUBLIC ${RpolyPlusPlus}
        PUBLIC fieldopt::ertwrapper
        PUBLIC fieldopt::reservoir
        PUBLIC fieldopt::utilities)

target_include_directories(wellindexcalculator PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/geometry_functions>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/well_constraint_projections>)

target_compile_features(wellindexcalculator
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
add_executable(test_wellindexcalculator
        tests/test_geometryfunctions.cpp
        tests/test_intersected_cells.cpp
        tests/test_single_cell_wellindex.cpp
        tests/well_constraint_projections_tests.cpp
        tests/test_domain_boundary.cpp
        )
target_link_libraries(test_wellindexcalculator
        fieldopt::wellindexcalculator
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_wellindexcalculator COMMAND $<TARGET_FILE:test_wellindexcalculator>)
