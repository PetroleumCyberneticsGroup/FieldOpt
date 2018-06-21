cmake_minimum_required(VERSION 2.8)
project(optimization)

# Dbg ======================================================
message("\n================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Options ==================================================
set(EXPERIMENTAL_CONSTRAINTS_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/experimental_constraints)
set(EXPERIMENTAL_CONSTRAINTS_SRC "")
set(EXPERIMENTAL_CONSTRAINTS_TESTS "")

if(EXISTS ${EXPERIMENTAL_CONSTRAINTS_DIR}
    AND IS_DIRECTORY ${EXPERIMENTAL_CONSTRAINTS_DIR})
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

# Add library ==============================================
add_library(optimization
    # CASE
    case.cpp
    case_handler.cpp
    case_transfer_object.cpp
    # OPTIMIZER
    optimizer.cpp
    optimizers/ExhaustiveSearch2DVert.cpp
    optimizers/GSS.cpp
    optimizers/compass_search.cpp
    optimizers/APPS.cpp
    optimizers/GeneticAlgorithm.cpp
    optimizers/RGARDD.cpp
    optimizers/DFO.cpp
    optimizers/SNOPTSolverC.cpp
    optimizers/OptimizationProblem.cpp
    optimizers/Subproblem.cpp
    # CONSTRAINTS
    constraints/constraint_handler.cpp
    constraints/constraint.cpp
    constraints/wcntrl_bhp.cpp
    constraints/res_ijk_box.cpp
    constraints/wspln_lngth.cpp
    constraints/wspln_interw_dist_anl.cpp
    constraints/constraint_wellspln.cpp
    constraints/c-wspln_lngth-interw_dist.cpp
    constraints/wcntrl_rate.cpp
    constraints/c-wspln_lngth-interw_dist-res_ijk_box.cpp
    constraints/wvert_pseudo_cont_2d_ijk_box.cpp
    constraints/wspln_interw_dist_opt.cpp
    constraints/constraint_adgprs_solver.cpp
    ${EXPERIMENTAL_CONSTRAINTS_SRC}
    # OBJECTIVES
    objective/objective.cpp
    objective/weightedsum.cpp
    normalizer.cpp
    optimizers/bayesian_optimization/AcquisitionFunction.cpp
    optimizers/bayesian_optimization/af_optimizers/AFPSO.cpp
    optimizers/bayesian_optimization/af_optimizers/AFOptimizer.cpp
    optimizers/bayesian_optimization/EGO.cpp
    optimizers/bayesian_optimization/af_optimizers/AFCompassSearch.cpp
    # AUX
    optimization_exceptions.h
    # SNOPT
    ../FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.cpp
    ../FieldOpt-3rdPartySolvers/handlers/LibraryHandler.c
    ../FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.c
    )

add_library(fieldopt::optimization ALIAS ${PROJECT_NAME})

# Target link libraries ====================================
target_link_libraries(optimization
    PUBLIC fieldopt::model
    PUBLIC fieldopt::simulation
    PUBLIC fieldopt::constraintmath
    PUBLIC fieldopt::reservoir
    PUBLIC fieldopt::runner
    PUBLIC ${gp}
    PUBLIC Qt5::Core
    PUBLIC fieldopt::3rdpartysolvers
    ${CMAKE_SOURCE_DIR}/FieldOpt-3rdPartySolvers/lib/libsnopt-7.2.12.2.so
    PUBLIC ${Boost_LIBRARIES})

# Target include directories ===============================
target_include_directories(optimization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

# Compile options ==========================================
add_compile_options(-std=c++11)

# Unit tests ===============================================
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
      tests/optimizers/test_ego.cpp
      tests/constraints/test_pseudo_cont_boundary_2d.cpp
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

# Install targets ==========================================
install(TARGETS optimization
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

# Debug: include dirs ======================================
include(../InclDirDbg.cmake)