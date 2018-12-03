cmake_minimum_required(VERSION 2.8)
project(model)

add_library(model STATIC
		cpp-spline/src/Vector.cpp
		cpp-spline/src/Curve.cpp
		cpp-spline/src/Bezier.cpp
		cpp-spline/src/CatmullRom.cpp
		cpp-spline/src/BSpline.cpp
		model.cpp
		wells/well.cpp
		wells/well_exceptions.h
		wells/wellbore/completions/completion.cpp
		wells/wellbore/completions/perforation.cpp
		wells/wellbore/trajectory.cpp
		wells/wellbore/wellspline.cpp
		wells/control.cpp
		wells/wellbore/wellblock.cpp
		properties/property.h
		properties/property_exceptions.h
		properties/binary_property.cpp
		properties/variable_property_container.cpp
		properties/continous_property.cpp
		properties/discrete_property.cpp
        properties/property.cpp
        model_synchronization_object.cpp
		wells/wellbore/pseudo_cont_vert.cpp
		wells/wellbore/completions/segmented_completion.cpp
		wells/wellbore/completions/packer.cpp
		wells/wellbore/completions/icd.cpp
		wells/compartment.cpp
		wells/segment.cpp
)
add_library(fieldopt::model ALIAS ${PROJECT_NAME})

target_link_libraries (model
        PUBLIC fieldopt::reservoir
		PUBLIC fieldopt::wellindexcalculator
		PUBLIC fieldopt::settings
		PUBLIC fieldopt::constraintmath
		PUBLIC fieldopt::runner
		PUBLIC Qt5::Core
		PUBLIC ${Boost_LIBRARIES})

target_include_directories(model PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/properties>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore/completions>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
    add_executable(test_model
            tests/test_model.cpp
            tests/variables/test_properties.cpp
            tests/variables/test_variable_property_container.cpp
            tests/wells/test_control.cpp
            tests/wells/test_perforation.cpp
            tests/wells/test_trajectory.cpp
            tests/wells/test_well.cpp
            tests/wells/test_wellblock.cpp
            tests/test_resource_variable_property_container.h
            tests/test_resource_model.h
			tests/wells/test_pseudo_cont_vert.cpp
			tests/wells/test_block_to_spline_conversion.cpp
			tests/wells/test_segmented_well.cpp tests/wells/test_trajectory_curve.cpp)
    target_link_libraries(test_model
            fieldopt::model
            fieldopt::reservoir
            fieldopt::optimization
            ${gtest}
			${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_model COMMAND $<TARGET_FILE:test_model>)
endif()

install( TARGETS model
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
		)
