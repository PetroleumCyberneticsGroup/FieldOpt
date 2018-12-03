cmake_minimum_required( VERSION 2.8 )
project(simulation)

add_library(simulation STATIC
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
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_section.cpp
        results/results.h
        results/results_exceptions.h
        results/eclresults.cpp
        results/adgprsresults.cpp
        simulator_interfaces/driver_file_writers/adgprsdriverfilewriter.cpp
        simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/wellstre.cpp
        simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/adgprs_wellcontrols.cpp
        simulator_interfaces/driver_file_writers/flowdriverfilewriter.cpp
        simulator_interfaces/flowsimulator.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/welsegs.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/compsegs.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/wsegvalv.cpp
        simulator_interfaces/driver_file_writers/driver_parts/ix_driver_parts/flow_control_device.hpp
        simulator_interfaces/driver_file_writers/ix_driver_file_writer.cpp simulator_interfaces/ix_simulator.cpp simulator_interfaces/ix_simulator.h)
add_library(fieldopt::simulation ALIAS ${PROJECT_NAME} )

target_link_libraries(simulation
        PUBLIC fieldopt::model
        PUBLIC ${Boost_LIBRARIES}
        PUBLIC fieldopt::hdf5summaryreader)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
    add_executable(test_simulation
            tests/results/test_adgprsresults.cpp
            tests/results/test_eclresults.cpp
            tests/simulator_interfaces/test_adgprssimulator.cpp
            tests/simulator_interfaces/test_eclsimulator.cpp
            tests/simulator_interfaces/driver_file_writers/adgprs_driver_file_writer.cpp
            tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_compdat.cpp
            tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_schedule_section.cpp
            tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_wellcontrols.cpp
            tests/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_welspecs.cpp
            tests/test_resource_results.h
            tests/simulator_interfaces/driver_file_writers/flow_driver_file_writer.cpp tests/simulator_interfaces/test_ix_simulator.cpp)
    target_link_libraries(test_simulation
            fieldopt::simulation
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_simulation COMMAND $<TARGET_FILE:test_simulation>)
endif()

install( TARGETS simulation
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
