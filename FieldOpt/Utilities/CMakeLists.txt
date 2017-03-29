cmake_minimum_required(VERSION 3.2)
project(utilities LANGUAGES CXX)


include_directories(utilities PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(test_utilities
            tests/test_filehandling.cpp
            tests/test_execution.cpp
            tests/test_time.cpp
            tests/test_math.cpp)
    target_link_libraries(test_utilities
            Qt5::Core
            ${Boost_LIBRARIES}
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    target_compile_features(test_utilities
            PUBLIC cxx_auto_type
            PUBLIC cxx_range_for)

    add_test(NAME test_utilities COMMAND $<TARGET_FILE:test_utilities>)
endif()
