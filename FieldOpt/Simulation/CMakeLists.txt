cmake_minimum_required( VERSION 3.2 )
project(simulation LANGUAGES CXX)

add_library(simulation
        simulator_interfaces/simulator_exceptions.h
        simulator_interfaces/simulator.cpp
        simulator_interfaces/driver_file_writers/ecldriverfilewriter.cpp
        simulator_interfaces/eclsimulator.cpp
        simulator_interfaces/adgprssimulator.cpp
        simulator_interfaces/driver_file_writers/driver_parts/driverpart.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/welspecs.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/ecldriverpart.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/compdat.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/wellcontrols.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/grid_section.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/runspec_section.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/props_section.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/solution_section.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/summary_section.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_section.cpp
        results/results.h
        results/results_exceptions.h
        results/eclresults.cpp
        results/adgprsresults.cpp
        simulator_interfaces/driver_file_writers/adgprsdriverfilewriter.cpp
        simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/wellstre.cpp
        simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/adgprs_wellcontrols.cpp)
add_library(fieldopt::simulation ALIAS ${PROJECT_NAME})

target_link_libraries(simulation
        PUBLIC fieldopt::model
        PUBLIC fieldopt::adgprsresultsreader)

target_compile_features(ertwrapper
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)
