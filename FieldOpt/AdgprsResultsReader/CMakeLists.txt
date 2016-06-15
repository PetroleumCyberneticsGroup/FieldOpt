cmake_minimum_required(VERSION 3.2)
project(adgprsresultsreader)

add_library(adgprsresultsreader
		json_summary_reader.cpp
		adgprs_results_reader.cpp)
add_library(fieldopt::adgprsresultsreader ALIAS ${PROJECT_NAME})

target_link_libraries(adgprsresultsreader
		PUBLIC Qt5::Core)

target_include_directories(adgprsresultsreader PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(adgprsresultsreader
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(test_adgprsresultsreader
		tests/test_adgprs_results_reader.cpp
		tests/test_json_summary_reader.cpp
		)
target_link_libraries(test_adgprsresultsreader
		fieldopt::adgprsresultsreader
        fieldopt::utilities
        fieldopt::model
		${GTEST_BOTH_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_adgprsresultsreader COMMAND $<TARGET_FILE:test_adgprsresultsreader>)
