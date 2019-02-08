cmake_minimum_required( VERSION 2.8 )
project(simulation)

include(Sources.cmake)

add_library(simulation STATIC ${SIMULATION_HEADERS} ${SIMULATION_SOURCES})
add_library(fieldopt::simulation ALIAS ${PROJECT_NAME})

target_link_libraries(simulation
        PUBLIC fieldopt::model
        PUBLIC ${Boost_LIBRARIES}
        PUBLIC fieldopt::hdf5summaryreader)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
	add_executable(test_simulation ${SIMULATION_TESTS})
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
