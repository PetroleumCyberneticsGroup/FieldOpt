cmake_minimum_required(VERSION 3.2)
project(FieldOpt LANGUAGES CXX)

# Qt libraries
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)


# Boost libraries
find_package(Boost REQUIRED program_options)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
add_library(boost-program_options SHARED IMPORTED)
set_property(TARGET boost-program_options PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-program_options PROPERTY IMPORTED_LOCATION ${Boost_PROPGRAM_OPTIONS_LIBRARY})

find_package(Armadillo)
find_library(RpolyPlusPlus rpoly_plus_plus REQUIRED)

# Set this directory as the root dir
include_directories(${CMAKE_SOURCE_DIR})

#enable_testing()


add_subdirectory(ERTWrapper)
add_subdirectory(Model)
add_subdirectory(Utilities)
#add_subdirectory(Optimization)
add_subdirectory(Simulation)
#add_subdirectory(Runner)
add_subdirectory(WellIndexCalculator)
#add_subdirectory(WellIndexCalculator/WellIndexCalculatorTests)
#add_subdirectory(GTest)
add_subdirectory(AdgprsResultsReader)
