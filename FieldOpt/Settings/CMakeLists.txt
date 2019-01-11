cmake_minimum_required(VERSION 2.8)
project(settings)

include(Sources.cmake)

add_library(settings STATIC ${SETTINGS_HEADERS} ${SETTINGS_SOURCES})
add_library(fieldopt::settings ALIAS ${PROJECT_NAME})

target_link_libraries(settings
#        PUBLIC ${ERTLIB}
        PUBLIC ri:ert_ecl
        PUBLIC ${ert_util}
        PUBLIC ${opm-common_LIBRARIES}
        PUBLIC ${Boost_LIBRARIES}
        PUBLIC Qt5::Core)

target_include_directories(settings PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
	add_executable(test_settings ${SETTINGS_TESTS})
    target_link_libraries(test_settings
            fieldopt::settings
            ${Boost_LIBRARIES}
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_settings COMMAND $<TARGET_FILE:test_settings>)
endif()

install( TARGETS settings
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
