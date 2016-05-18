cmake_minimum_required(VERSION 3.2)
project(optimization LANGUAGES CXX)

add_library(optimization
        optimization_exceptions.h
		objective/objective.cpp
		objective/weightedsum.cpp
		case.cpp
		case_handler.cpp
		constraints/constraint.cpp
		constraints/box_constraint.cpp
		constraints/constraint_handler.cpp
		optimizer.cpp
		optimizers/compass_search.cpp
		constraints/well_spline_length.cpp
		constraints/interwell_distance.cpp
		constraints/well_spline_constraint.cpp
		constraints/combined_spline_length_interwell_distance.cpp)
add_library(fieldopt::optimization ALIAS ${PROJECT_NAME})

target_link_libraries(optimization
        PUBLIC fieldopt::model
        PUBLIC fieldopt::simulation
        PUBLIC fieldopt::wellindexcalculator)

target_include_directories(optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

target_compile_features(ertwrapper
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} tests)
add_executable(test_optimization
        tests/test_fixture_optimizer.h
        tests/test_fixture_case.h
        tests/test_case.cpp
        tests/test_case_handler.cpp
        tests/constraints/test_box_constraint.cpp
        tests/constraints/test_constraint_handler.cpp
        tests/objective/test_weightedsum.cpp
        tests/optimizers/test_compass_search.cpp
        )

target_link_libraries(test_optimization
        fieldopt::optimization
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(test_optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>)

add_test(test_optimization test_optimization)
