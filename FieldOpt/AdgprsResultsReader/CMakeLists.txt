project(adgprsresultsreader)
cmake_minimum_required(VERSION 3.2)

add_library(adgprsresultsreader
		json_summary_reader.cpp
		adgprs_results_reader.cpp)
add_library(fieldopt::adgprsresultsreader ALIAS ${PROJECT_NAME})

target_link_libraries(adgprsresultsreader
		PUBLIC Qt5::Core)

target_include_directories(adgprsresultsreader PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/)

target_compile_features(adgprsresultsreader
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)