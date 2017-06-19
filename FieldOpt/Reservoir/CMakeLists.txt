cmake_minimum_required(VERSION 2.8)
project(reservoir)

add_library(reservoir
        grid/cell.cpp
        grid/grid.cpp
        grid/ijkcoordinate.cpp
        grid/eclgrid.cpp)

add_library(fieldopt::reservoir ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/libraries/include/)
target_link_libraries (reservoir
        PUBLIC fieldopt::ertwrapper
        ${Boost_LIBRARIES})

add_compile_options(-std=c++11)

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
        tests/grid/test_ijkcoordinate.cpp
        )
target_link_libraries(test_reservoir
        fieldopt::reservoir
        fieldopt::ertwrapper
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

    set_target_properties(test_reservoir
            PROPERTIES COMPILE_FLAGS "-fPIC")

    add_test(NAME test_reservoir COMMAND $<TARGET_FILE:test_reservoir>)
endif()

install( TARGETS reservoir
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
