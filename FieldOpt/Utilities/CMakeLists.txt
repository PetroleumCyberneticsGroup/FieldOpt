cmake_minimum_required(VERSION 2.8)
project(utilities)

include(Sources.cmake)

include_directories(utilities PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if (BUILD_TESTING)
    # Unit tests
	add_executable(test_utilities ${UTILITIES_TESTS})
    target_link_libraries(test_utilities
            Qt5::Core
            ${Boost_LIBRARIES}
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_compile_options(-std=c++11)

    add_test(NAME test_utilities COMMAND $<TARGET_FILE:test_utilities>)
endif()
