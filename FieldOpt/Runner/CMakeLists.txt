cmake_minimum_required(VERSION 2.8)
project(runner)

include(Sources.cmake)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(FieldOpt main.cpp ${RUNNER_HEADERS} ${RUNNER_SOURCES})

add_library(runner STATIC ${RUNNER_HEADERS} ${RUNNER_SOURCES})
add_library(fieldopt::runner ALIAS runner)

target_link_libraries(FieldOpt
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::settings
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::hdf5summaryreader
		PUBLIC fieldopt::constraintmath
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES}
        PUBLIC ${MPI_LIBRARIES}
		PUBLIC ri:ert_ecl
		)

target_link_libraries(runner
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::settings
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::hdf5summaryreader
		PUBLIC fieldopt::constraintmath
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ri:ert_ecl
		)

if(MPI_COMPILE_FLAGS)
	set_target_properties(runner PROPERTIES
			COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
	set_target_properties(runner PROPERTIES
			LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()


target_include_directories(FieldOpt PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/runners>)

target_include_directories(runner PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/runners>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
	# Unit tests
	add_executable(test_runner ${RUNNER_TESTS})
	target_link_libraries(test_runner
			fieldopt::runner
			${gtest}
			${gtest_main}
			${CMAKE_THREAD_LIBS_INIT})

	add_test(NAME test_runner COMMAND $<TARGET_FILE:test_runner>)
endif()

install(TARGETS FieldOpt runner
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
)

