cmake_minimum_required(VERSION 3.2)
project(runner LANGUAGES CXX)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(FieldOpt
		main.cpp
		runtime_settings.cpp
		runners/main_runner.cpp
		runners/abstract_runner.cpp
		runners/serial_runner.cpp
        runners/oneoff_runner.cpp
		logger.cpp
		bookkeeper.cpp)

add_library(runner
		runtime_settings.cpp
		runners/main_runner.cpp
		runners/abstract_runner.cpp
		runners/serial_runner.cpp
        runners/oneoff_runner.cpp
		logger.cpp
		bookkeeper.cpp)
add_library(fieldopt::runner ALIAS runner)

target_link_libraries(FieldOpt
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::utilities
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::hdf5summaryreader
		PUBLIC fieldopt::constraintmath
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES}
        PUBLIC ${MPI_LIBRARIES})

target_link_libraries(runner
		PUBLIC fieldopt::model
		PUBLIC fieldopt::simulation
		PUBLIC fieldopt::optimization
		PUBLIC fieldopt::utilities
		PUBLIC fieldopt::ertwrapper
		PUBLIC fieldopt::hdf5summaryreader
		PUBLIC fieldopt::constraintmath
		PUBLIC ${CMAKE_THREAD_LIBS_INIT}
		PUBLIC ${Boost_LIBRARIES})

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
        tests/test_bookkeeper.cpp
        )
target_link_libraries(test_runner
        fieldopt::runner
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_runner COMMAND $<TARGET_FILE:test_runner>)
