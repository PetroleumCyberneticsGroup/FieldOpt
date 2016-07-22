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
        simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/adgprs_wellcontrols.cpp
        simulator_interfaces/driver_file_writers/flowdriverfilewriter.cpp
        simulator_interfaces/flowsimulator.cpp)
add_library(fieldopt::simulation ALIAS ${PROJECT_NAME})

target_link_libraries(simulation
        PUBLIC fieldopt::model
        PUBLIC fieldopt::hdf5summaryreader)

target_compile_features(ertwrapper
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(test_simulation
        tests/results/test_adgprsresults.cpp
        tests/results/test_eclresults.cpp
        tests/simulator_interfaces/test_adgprssimulator.cpp
        tests/simulator_interfaces/test_eclsimulator.cpp
        tests/simulator_interfaces/driver_file_writers/adgprs_driver_file_writer.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_compdat.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_grid_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_props_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_runspec_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_schedule_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_solution_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_summary_section.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_wellcontrols.cpp
        tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_welspecs.cpp
        tests/test_resource_results.h
        tests/simulator_interfaces/driver_file_writers/flow_driver_file_writer.cpp)
target_link_libraries(test_simulation
        fieldopt::simulation
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_simulation COMMAND $<TARGET_FILE:test_simulation>)
