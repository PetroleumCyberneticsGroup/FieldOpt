cmake_minimum_required(VERSION 2.8)
project(model)

include(Sources.cmake)

add_library(model STATIC ${MODEL_HEADERS} ${MODEL_SOURCES})
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
	add_executable(test_model ${MODEL_TESTS})
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
