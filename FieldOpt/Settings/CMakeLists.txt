cmake_minimum_required(VERSION 2.8)
project(settings)

add_library(settings
        settings_exceptions.h
        optimizer.cpp
        simulator.cpp
        model.cpp
        settings.cpp
        )
add_library(fieldopt::settings ALIAS ${PROJECT_NAME})

target_link_libraries(settings
        PUBLIC Qt5::Core)

target_include_directories(settings PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(test_settings
            tests/test_settings.cpp
            tests/test_settings_model.cpp
            tests/test_settings_optimizer.cpp
            tests/test_settings_simulator.cpp
            )
    target_link_libraries(test_settings
            fieldopt::settings
            ${Boost_LIBRARIES}
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_settings COMMAND $<TARGET_FILE:test_settings>)
endif()

install( TARGETS settings
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
