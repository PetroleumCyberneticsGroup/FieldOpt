cmake_minimum_required(VERSION 3.2)
project(ertwrapper)

# ERT libraries
find_library(ert_ecl
        NAMES ecl
        HINTS ${CMAKE_BINARY_DIR}/libraries/lib
        REQUIRED)
find_library(ert_util
        NAMES ert_util
        HINTS ${CMAKE_BINARY_DIR}/libraries/lib
        REQUIRED)
include_directories(${CMAKE_BINARY_DIR}/libraries/include/)

add_library(ertwrapper
        ertwrapper_exceptions.h
        eclgridreader.cpp
        eclsummaryreader.cpp)
add_library(fieldopt::ertwrapper ALIAS ${PROJECT_NAME})

include_directories(
        ${EIGEN3_INCLUDE_DIR}
	${ERT_INCLUDE_DIR})

target_link_libraries(ertwrapper
        PUBLIC ${ert_ecl}
        PUBLIC ${ert_util})

target_include_directories(ertwrapper PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(ertwrapper
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)

# Enable position independent code when compiling lib, so that
# shared library libwellindexcalculator.so may be linked with
# static reservoir and ertwrapper libraries
if (BUILD_WIC_ADGPRS)
    set_target_properties(ertwrapper
            PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

# Unit tests
if (BUILD_TESTING)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(test_ertwrapper
            tests/test_eclgridreader.cpp
            tests/test_eclsummaryreader.cpp
            )
    target_link_libraries(test_ertwrapper
            fieldopt::ertwrapper
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_ertwrapper COMMAND $<TARGET_FILE:test_ertwrapper>)
endif()

install( TARGETS ertwrapper
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
