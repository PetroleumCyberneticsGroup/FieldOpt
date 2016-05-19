cmake_minimum_required(VERSION 3.2)
project(model LANGUAGES CXX)

add_library(model
		model.cpp
		wells/well.cpp
		reservoir/reservoir.cpp
		reservoir/grid/grid_exceptions.h
		reservoir/grid/cell.cpp
		reservoir/grid/grid.cpp
		reservoir/grid/ijkcoordinate.cpp
		reservoir/grid/xyzcoordinate.cpp
		wells/well_exceptions.h
		wells/wellbore/completions/completion.cpp
		wells/wellbore/completions/perforation.cpp
		wells/wellbore/trajectory.cpp
		wells/wellbore/wellspline.cpp
		reservoir/grid/eclgrid.cpp
		wells/control.cpp
		wells/wellbore/wellblock.cpp
		properties/property.h
		properties/property_exceptions.h
		properties/binary_property.cpp
		properties/variable_property_container.cpp
		properties/variable_property_handler.cpp
		properties/continous_property.cpp
		properties/discrete_property.cpp)
add_library(fieldopt::model ALIAS ${PROJECT_NAME})

target_link_libraries (model
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::utilities
		PUBLIC fieldopt::wellindexcalculator
		PUBLIC Qt5::Core)

target_include_directories(model PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/properties>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reservoir>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reservoir/grid>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore/completions>)

target_compile_features(model
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} tests)
add_executable(test_model
		tests/test_fixture_model_base.h
		tests/test_model.cpp
		tests/reservoir/test_reservoir.cpp
		tests/reservoir/grid/test_cell.cpp
		tests/reservoir/grid/test_grid.cpp
		tests/reservoir/grid/test_ijkcoordinate.cpp
		tests/reservoir/grid/test_xyzcoordinate.cpp
		tests/results/test_eclresults.cpp
		tests/variables/test_properties.cpp
		tests/variables/test_variable_property_handler.cpp
		tests/variables/test_variable_property_container.cpp
		tests/wells/test_control.cpp
		tests/wells/test_perforation.cpp
		tests/wells/test_trajectory.cpp
		tests/wells/test_well.cpp
		tests/wells/test_wellblock.cpp
		)
target_link_libraries(test_model
		fieldopt::model
		fieldopt::optimization
		${GTEST_BOTH_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_model COMMAND $<TARGET_FILE:test_model>)
