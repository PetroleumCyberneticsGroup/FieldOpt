cmake_minimum_required(VERSION 3.2)
project(reservoir LANGUAGES CXX)

add_library(reservoir
        grid/cell.cpp
        grid/grid.cpp
        grid/ijkcoordinate.cpp
        grid/eclgrid.cpp)

add_library(fieldopt::reservoir ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries (reservoir
        PUBLIC fieldopt::ertwrapper
        ${Boost_LIBRARIES})

target_compile_features(reservoir
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Enable position independent code when compiling lib, so that
# shared library libwellindexcalculator.so may be linked with
# static reservoir and ertwrapper libraries
if (BUILD_WIC_ADGPRS)
    set_target_properties(reservoir
            PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} tests)
    add_executable(test_reservoir
            tests/test_resource_grids.h
            tests/grid/test_cell.cpp
            tests/grid/test_grid.cpp
            tests/grid/test_ijkcoordinate.cpp)
    target_link_libraries(test_reservoir
            fieldopt::reservoir
            ${Boost_LIBRARIES}
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_reservoir COMMAND $<TARGET_FILE:test_reservoir>)
endif()
