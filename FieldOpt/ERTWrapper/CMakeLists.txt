cmake_minimum_required(VERSION 2.8)
project(ertwrapper)

include(Sources.cmake)


add_library(ertwrapper STATIC ${ERTWRAPPER_HEADERS} ${ERTWRAPPER_SOURCES})
add_library(fieldopt::ertwrapper ALIAS ${PROJECT_NAME})

include_directories(
        ${EIGEN3_INCLUDE_DIR}
	${ERT_INCLUDE_DIR})

target_link_libraries(ertwrapper
        PUBLIC ri:ert_ecl
        )

target_include_directories(ertwrapper PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_compile_options(-std=c++11)

# Enable position independent code when compiling lib, so that
# shared library libwellindexcalculator.so may be linked with
# static reservoir and ertwrapper libraries
if (BUILD_WIC_ADGPRS)
    set_target_properties(ertwrapper
            PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

# Unit tests
if (BUILD_TESTING)
	add_executable(test_ertwrapper ${ERTWRAPPER_TESTS})
    target_link_libraries(test_ertwrapper
            fieldopt::ertwrapper
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_ertwrapper COMMAND $<TARGET_FILE:test_ertwrapper>)
endif()

install( TARGETS ertwrapper
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
