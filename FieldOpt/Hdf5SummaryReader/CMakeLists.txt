cmake_minimum_required( VERSION 3.2 )
project(hdf5summaryreader LANGUAGES CXX)

find_package(HDF5 REQUIRED COMPONENTS CXX)
add_definitions(${HDF5_DEFINITIONS})
include_directories(${HDF5_INCLUDE_DIRS})

add_library(hdf5summaryreader
        hdf5_summary_reader.cpp)

add_library(fieldopt::hdf5summaryreader ALIAS ${PROJECT_NAME})

target_link_libraries(hdf5summaryreader
        PUBLIC ${HDF5_LIBRARIES})

target_compile_features(hdf5summaryreader
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Unit tests
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
add_executable(test_hdf5summaryreader
        tests/test_hdf5_summary_reader.cpp)

target_link_libraries(test_hdf5summaryreader
        fieldopt::hdf5summaryreader
        ${HDF5_LIBRARIES}
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME test_hdf5summaryreader COMMAND $<TARGET_FILE:test_hdf5summaryreader>)
