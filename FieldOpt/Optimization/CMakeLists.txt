cmake_minimum_required(VERSION 2.8)
project(optimization)

add_library(optimization STATIC
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
		optimizers/bayesian_optimization/AcquisitionFunction.cpp
		optimizers/bayesian_optimization/af_optimizers/AFPSO.cpp
		optimizers/bayesian_optimization/af_optimizers/AFOptimizer.cpp
        optimizers/bayesian_optimization/EGO.cpp
        optimizers/bayesian_optimization/af_optimizers/AFCompassSearch.cpp
		constraints/pseudo_cont_boundary_2d.cpp
		constraints/icv_constraint.cpp
        constraints/packer_constraint.cpp
        hybrid_optimizer.cpp
        objective/NPV.cpp objective/NPV.h)
add_library(fieldopt::optimization ALIAS ${PROJECT_NAME})

target_link_libraries(optimization
        PUBLIC fieldopt::model
        PUBLIC fieldopt::simulation
        PUBLIC fieldopt::constraintmath
		PUBLIC fieldopt::reservoir
        PUBLIC fieldopt::runner
		PUBLIC ${gp}
        Qt5::Core
		PUBLIC ${Boost_LIBRARIES})

target_include_directories(optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
	# Unit tests
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
			tests/constraints/test_rate_constraint.cpp
			tests/test_case_transfer_object.cpp
            tests/optimizers/test_apps.cpp
            tests/optimizers/test_ga.cpp
			tests/test_normalizer.cpp
            tests/constraints/test_interwell_distance.cpp
            tests/constraints/test_spline_well_length.cpp
			tests/optimizers/test_ego.cpp
            tests/constraints/test_pseudo_cont_boundary_2d.cpp)


	target_link_libraries(test_optimization
			fieldopt::optimization
			${gtest}
			${gtest_main}
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
