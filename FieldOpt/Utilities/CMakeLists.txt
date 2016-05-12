cmake_minimum_required(VERSION 3.2)
project(utilities LANGUAGES CXX)

#set(CMAKE_AUTOMOC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#find_package (Qt5Core REQUIRED)

#include_directories(${CMAKE_SOURCE_DIR}/..)

add_library(utilities
		settings/optimizer.cpp
		settings/simulator.cpp
		settings/model.cpp
		settings/settings.cpp
		file_handling/filehandling.cpp
		unix/execution.cpp)
add_library(fieldopt::utilities ALIAS ${PROJECT_NAME})
qt5_use_modules(utilities Core)

target_include_directories(utilities PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/settings>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/file_handling>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/unix>
		PRIVATE src)

target_compile_features(utilities
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)
