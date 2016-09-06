cmake_minimum_required(VERSION 3.2)
project(utilities LANGUAGES CXX)

add_library(utilities
        settings/settings_exceptions.h
		settings/optimizer.cpp
		settings/simulator.cpp
		settings/model.cpp
		settings/settings.cpp
        file_handling/filehandling_exceptions.h
		file_handling/filehandling.cpp
		unix/execution.cpp)
add_library(fieldopt::utilities ALIAS ${PROJECT_NAME})

target_link_libraries(utilities
        PUBLIC Qt5::Core)

target_include_directories(utilities PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/file_handling>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/unix>)

target_compile_features(utilities
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(test_utilities
        tests/test_resource_example_file_paths.hpp
		tests/test_resource_settings.hpp
        tests/file_handling/test_filehandling.cpp
        tests/settings/test_settings.cpp
        tests/settings/test_settings_model.cpp
        tests/settings/test_settings_optimizer.cpp
        tests/settings/test_settings_simulator.cpp
        tests/unix/test_execution.cpp
		tests/test_time.cpp tests/test_math.cpp)
target_link_libraries(test_utilities
        fieldopt::utilities
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_utilities COMMAND $<TARGET_FILE:test_utilities>)
