cmake_minimum_required(VERSION 3.2)
project(utilities LANGUAGES CXX)
#set (CMAKE_BUILD_TYPE Debug)

find_package (Qt5 REQUIRED COMPONENTS Core)

add_library(utilities
		src/settings/optimizer.cpp
		src/settings/simulator.cpp
		src/settings/model.cpp
		src/settings/settings.cpp
		src/file_handling/filehandling.cpp
		src/unix/execution.cpp)
add_library(fieldopt::utilities ALIAS ${PROJECT_NAME})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(utilities PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
		PRIVATE src)

# If we have compiler requirements for this library, list them
target_compile_features(utilities
		PUBLIC cxx_auto_type
		PUBLIC cxx_range_for)

# Depend on a library that we defined in the top-level file
target_link_libraries(utilities
		Qt5::Core)

# 'make install' to the correct location
install(TARGETS utilities
		ARCHIVE DESTINATION utilities
		LIBRARY DESTINATION utilities)
install(DIRECTORY include/ DESTINATION include)

# This makes the project importable from the build directory
export(TARGETS utilities FILE UtilitiesConfig.cmake)