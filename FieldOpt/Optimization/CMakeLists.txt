cmake_minimum_required(VERSION 3.2)
project(optimization LANGUAGES CXX)

set(EXPERIMENTAL_CONSTRAINTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/experimental_constraints)
set(EXPERIMENTAL_CONSTRAINTS_SRC "")
set(EXPERIMENTAL_CONSTRAINTS_TESTS "")
if(EXISTS ${EXPERIMENTAL_CONSTRAINTS_DIR} AND IS_DIRECTORY ${EXPERIMENTAL_CONSTRAINTS_DIR})
	message("Including experimental constraints")
	add_definitions(-DWITH_EXPERIMENTAL_CONSTRIANTS)
	set(EXPERIMENTAL_CONSTRAINTS_SRC
			${EXPERIMENTAL_CONSTRAINTS_DIR}/)
	set(EXPERIMENTAL_CONSTRAINTS_TESTS
			${EXPERIMENTAL_CONSTRAINTS_DIR}/tests/)
else()
	message("Experimental constraints not found")
	add_definitions(-DWITHOUT_EXPERIMENTAL_CONSTRAINTS)
endif()

add_library(optimization
        optimization_exceptions.h
		objective/objective.cpp
		objective/weightedsum.cpp
		case.cpp
		case_handler.cpp
		constraints/constraint.cpp
		constraints/bhp_constraint.cpp
		constraints/constraint_handler.cpp
		constraints/reservoir_boundary.cpp
		optimizer.cpp
		constraints/well_spline_length.cpp
		constraints/interwell_distance.cpp
		constraints/well_spline_constraint.cpp
		constraints/combined_spline_length_interwell_distance.cpp
		constraints/rate_constraint.cpp
		constraints/combined_spline_length_interwell_distance_reservoir_boundary.cpp
		case_transfer_object.cpp
		optimizers/ExhaustiveSearch2DVert.cpp
		optimizers/GSS.cpp
		optimizers/compass_search.cpp
		optimizers/APPS.cpp
		optimizers/GeneticAlgorithm.cpp
		optimizers/RGARDD.cpp
        normalizer.cpp
		${EXPERIMENTAL_CONSTRAINTS_SRC})
add_library(fieldopt::optimization ALIAS ${PROJECT_NAME})

target_link_libraries(optimization
        PUBLIC fieldopt::model
        PUBLIC fieldopt::simulation
        PUBLIC fieldopt::constraintmath
		PUBLIC fieldopt::reservoir
        PUBLIC fieldopt::runner
		PUBLIC gp
        Qt5::Core
		PUBLIC ${Boost_LIBRARIES})

target_include_directories(optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

target_compile_features(optimization
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

if (BUILD_TESTING)
	# Unit tests
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS} tests)
	add_executable(test_optimization
			tests/test_case.cpp
			tests/test_case_handler.cpp
			tests/constraints/test_bhp_constraint.cpp
			tests/constraints/test_constraint_handler.cpp
			tests/constraints/test_reservoir_boundary.cpp
			tests/objective/test_weightedsum.cpp
			tests/optimizers/test_compass_search.cpp
			tests/test_resource_cases.h
			tests/test_resource_optimizer.h
			${EXPERIMENTAL_CONSTRAINTS_TESTS}
			tests/constraints/test_rate_constraint.cpp
			tests/test_case_transfer_object.cpp
            tests/optimizers/test_apps.cpp
            tests/optimizers/test_ga.cpp
			tests/test_normalizer.cpp
            tests/constraints/test_interwell_distance.cpp
            tests/constraints/test_spline_well_length.cpp
            )

	target_link_libraries(test_optimization
			fieldopt::optimization
			${GTEST_BOTH_LIBRARIES}
			${CMAKE_THREAD_LIBS_INIT})

	target_include_directories(test_optimization PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

	add_test(NAME test_optimization COMMAND $<TARGET_FILE:test_optimization>)
endif()

install( TARGETS optimization
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
		)
