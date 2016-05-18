cmake_minimum_required(VERSION 3.2)
project(runner LANGUAGES CXX)

add_executable(FieldOpt
		main.cpp
		runtime_settings.cpp
		runners/main_runner.cpp
		runners/abstract_runner.cpp
		runners/serial_runner.cpp
		logger.cpp
		bookkeeper.cpp)

add_library(runner
		runtime_settings.cpp
		runners/main_runner.cpp
		runners/abstract_runner.cpp
		runners/serial_runner.cpp
		logger.cpp
		bookkeeper.cpp)
add_library(fieldopt::runner ALIAS runner)

target_link_libraries(FieldOpt
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::utilities
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::adgprsresultsreader
		PUBLIC fieldopt::wellindexcalculator
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES})

target_link_libraries(runner
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::utilities
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::adgprsresultsreader
		PUBLIC fieldopt::wellindexcalculator
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES})

target_include_directories(FieldOpt PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/runners>)

target_include_directories(runner PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/runners>)

target_compile_features(FieldOpt
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

target_compile_features(runner
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(test_runner
        ../Optimization/tests/test_fixture_case.h
        tests/test_bookkeeper.cpp
        )
target_link_libraries(test_runner
        fieldopt::runner
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(test_runner test_runner)
