cmake_minimum_required(VERSION 2.8)
project(utilities)

# Dbg ==================================================================
message("\n============================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Include dirs =========================================================
include_directories(utilities PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Unit tests ===========================================================
if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(test_utilities
            tests/test_filehandling.cpp
            tests/test_execution.cpp
            tests/test_time.cpp
            tests/test_math.cpp colors.hpp)
    target_link_libraries(test_utilities
            Qt5::Core
            ${Boost_LIBRARIES}
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_compile_options(-std=c++11)

    add_test(NAME test_utilities COMMAND $<TARGET_FILE:test_utilities>)
endif()

# Debug: include dirs ==================================================
include(../InclDirDbg.cmake)