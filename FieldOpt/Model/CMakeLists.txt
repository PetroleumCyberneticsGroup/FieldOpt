cmake_minimum_required(VERSION 3.2)
project(model)

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
		wells/wellbore/completions/completion_exceptions.h
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
		PUBLIC Qt5::Core
		PUBLIC RPolyPlusPlus)

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
