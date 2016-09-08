cmake_minimum_required(VERSION 3.2)
project(reservoir LANGUAGES CXX)

add_library(reservoir
        grid/cell.cpp
        grid/grid.cpp
        grid/ijkcoordinate.cpp
        grid/eclgrid.cpp
        well_index_calculation/intersected_cell.cpp
        well_index_calculation/wellindexcalculator.cpp)

add_library(fieldopt::reservoir ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries (reservoir
        PUBLIC fieldopt::ertwrapper
        ${Boost_LIBRARIES})

target_compile_features(reservoir
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Standalone WIC executable
add_executable(WellIndexCalculator
        well_index_calculation/main.cpp)

include_directories(${EIGEN3_INCLUDE_DIR})

target_link_libraries(WellIndexCalculator
        fieldopt::reservoir
        ${Boost_LIBRARIES})

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} tests)
add_executable(test_reservoir
        tests/test_resource_grids.h
        tests/grid/test_cell.cpp
        tests/grid/test_grid.cpp
        tests/grid/test_ijkcoordinate.cpp
        tests/well_index_calculation/test_intersected_cells.cpp
        tests/well_index_calculation/test_single_cell_wellindex.cpp)
target_link_libraries(test_reservoir
        fieldopt::reservoir
        fieldopt::ertwrapper
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_reservoir COMMAND $<TARGET_FILE:test_reservoir>)


