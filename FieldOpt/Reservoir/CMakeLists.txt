cmake_minimum_required(VERSION 3.2)
project(reservoir LANGUAGES CXX)

add_library(reservoir
        reservoir.cpp
        grid/grid_exceptions.h
        grid/cell.cpp
        grid/grid.cpp
        grid/ijkcoordinate.cpp
        grid/eclgrid.cpp)

add_library(fieldopt::reservoir ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries (reservoir
        PUBLIC fieldopt::ertwrapper
        PUBLIC fieldopt::utilities
        PUBLIC Qt5::Core)

target_compile_features(reservoir
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} tests)
add_executable(test_reservoir
        tests/test_resource_grids.h
        tests/test_reservoir.cpp
        tests/grid/test_cell.cpp
        tests/grid/test_grid.cpp
        tests/grid/test_ijkcoordinate.cpp)
target_link_libraries(test_reservoir
        fieldopt::reservoir
        fieldopt::ertwrapper
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_reservoir COMMAND $<TARGET_FILE:test_reservoir>)


