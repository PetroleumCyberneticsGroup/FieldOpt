cmake_minimum_required(VERSION 3.2)
project(FieldOpt VERSION 0.3 LANGUAGES CXX)

# Qt libraries
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

# Boost libraries
find_package(Boost)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif ()
set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON) # enable multithreading
find_package(Boost COMPONENTS REQUIRED program_options)

find_package(Armadillo)
find_package(Threads)

# Set this directory as the root dir
include_directories(${CMAKE_SOURCE_DIR})

enable_testing()

add_subdirectory(ERTWrapper)
add_subdirectory(Model)
add_subdirectory(Utilities)
add_subdirectory(Optimization)
add_subdirectory(Simulation)
add_subdirectory(Runner)
add_subdirectory(WellIndexCalculator)
#add_subdirectory(GTest)
add_subdirectory(AdgprsResultsReader)

# Copy ADGPRS summary convertion python scripts
file(COPY ${CMAKE_SOURCE_DIR}/AdgprsSummaryConverter DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/execution_scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
