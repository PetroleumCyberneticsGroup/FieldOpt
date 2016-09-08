cmake_minimum_required(VERSION 3.2)
project(wellindexcalculator LANGUAGES CXX)

add_library(wellindexcalculator
        intersected_cell.cpp
        wellindexcalculator.cpp)

add_library(fieldopt::wellindexcalculator ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries (wellindexcalculator
        PUBLIC fieldopt::reservoir
        ${Boost_LIBRARIES})

target_compile_features(wellindexcalculator
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Standalone WIC executable
add_executable(WellIndexCalculator
        main.cpp)

include_directories(${EIGEN3_INCLUDE_DIR})

target_link_libraries(WellIndexCalculator
        fieldopt::wellindexcalculator
        ${Boost_LIBRARIES})

if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} tests)
    add_executable(test_wellindexcalculator
            tests/test_intersected_cells.cpp
            tests/test_single_cell_wellindex.cpp)
    target_link_libraries(test_wellindexcalculator
            fieldopt::wellindexcalculator
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_wellindexcalculator COMMAND $<TARGET_FILE:test_wellindexcalculator>)
endif()
