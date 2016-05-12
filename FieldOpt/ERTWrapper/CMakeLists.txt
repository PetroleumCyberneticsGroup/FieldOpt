project(ertwrapper)
cmake_minimum_required(VERSION 3.2)

# ERT libraries
find_library(ert_ecl ecl REQUIRED)
find_library(ert_util util REQUIRED)

add_library(ertwrapper
		eclgridreader.cpp
		eclsummaryreader.cpp)
add_library(fieldopt::ertwrapper ALIAS ${PROJECT_NAME})

target_link_libraries(ertwrapper
		PUBLIC Qt5::Core
        PUBLIC Qt5::Gui
		PRIVATE ert_ecl
		PRIVATE ert_util)

target_include_directories(ertwrapper PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(ertwrapper
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)
