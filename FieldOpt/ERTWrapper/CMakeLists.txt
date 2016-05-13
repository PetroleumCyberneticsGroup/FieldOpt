cmake_minimum_required(VERSION 3.2)
project(ertwrapper)

# ERT libraries
find_library(ert_ecl NAMES ecl REQUIRED)
find_library(ert_util NAMES util REQUIRED)

add_library(ertwrapper
        ertwrapper_exceptions.h
		eclgridreader.cpp
		eclsummaryreader.cpp)
add_library(fieldopt::ertwrapper ALIAS ${PROJECT_NAME})

target_link_libraries(ertwrapper
		PUBLIC Qt5::Core
        PUBLIC Qt5::Gui
		PUBLIC ${ert_ecl}
		PUBLIC ${ert_util})

target_include_directories(ertwrapper PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(ertwrapper
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)
