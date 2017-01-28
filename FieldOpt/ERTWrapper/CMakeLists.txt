cmake_minimum_required(VERSION 3.2)
project(ertwrapper)

# ERT libraries ================================================================
find_library(ert_ecl NAMES ecl REQUIRED)
find_library(ert_util NAMES ert_util REQUIRED)
find_library(ert_geometry NAMES ert_geometry REQUIRED)

if (_WIN32)
	find_library(ZLIB_LIBRARY NAMES libz.a z REQUIRED)
	find_library(SHLWAPI_LIBRARY NAMES Shlwapi)
endif()

add_library(ertwrapper
        ertwrapper_exceptions.h
		eclgridreader.cpp
		eclsummaryreader.cpp)
add_library(fieldopt::ertwrapper ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})

if (_WIN32)
	target_link_libraries(ertwrapper
			PUBLIC ${ert_ecl}
			PUBLIC ${ert_geometry}
			PUBLIC ${ert_util}
			PUBLIC ${ZLIB_LIBRARY})
else()
	target_link_libraries(ertwrapper
			PUBLIC ${ert_ecl}
			PUBLIC ${ert_util})
endif()

target_include_directories(ertwrapper PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(ertwrapper
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Unit tests ===================================================================
	if (BUILD_TESTING)
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
	add_executable(test_ertwrapper
			tests/test_eclgridreader.cpp
			tests/test_eclsummaryreader.cpp
			)
	target_link_libraries(test_ertwrapper
			fieldopt::ertwrapper
			${GTEST_BOTH_LIBRARIES}
			${CMAKE_THREAD_LIBS_INIT})

	add_test(NAME test_ertwrapper COMMAND $<TARGET_FILE:test_ertwrapper>)
endif()
