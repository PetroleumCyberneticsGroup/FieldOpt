cmake_minimum_required( VERSION 2.8 )
project(hdf5summaryreader)

include(Sources.cmake)

find_package(HDF5 REQUIRED COMPONENTS CXX)
add_definitions(${HDF5_DEFINITIONS})
include_directories(${HDF5_INCLUDE_DIRS})

add_library(hdf5summaryreader STATIC ${HDF5SUMMARYREADER_HEADERS} ${HDF5SUMMARYREADER_SOURCES})
add_library(fieldopt::hdf5summaryreader ALIAS ${PROJECT_NAME})

target_link_libraries(hdf5summaryreader
        PUBLIC ${HDF5_LIBRARIES})

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
    include_directories(${HDF5_INCLUDE_DIRS})
	add_executable(test_hdf5summaryreader ${HDF5SUMMARYREADER_TESTS})

    target_link_libraries(test_hdf5summaryreader
            fieldopt::hdf5summaryreader
            ${HDF5_LIBRARIES}
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_hdf5summaryreader COMMAND $<TARGET_FILE:test_hdf5summaryreader>)
endif()

install( TARGETS hdf5summaryreader
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
