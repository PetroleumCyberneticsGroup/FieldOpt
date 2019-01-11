cmake_minimum_required(VERSION 2.8)
project(constraintmath)

include(Sources.cmake)

include_directories(${CMAKE_BINARY_DIR}/libraries/include/)

add_library(constraintmath STATIC ${CONSTRAINTMATH_HEADERS} ${CONSTRAINTMATH_SOURCES})
add_library(fieldopt::constraintmath ALIAS ${PROJECT_NAME})

include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries(constraintmath
        PUBLIC Qt5::Core
        PUBLIC ${RpolyPlusPlus_LIBRARIES}
        PUBLIC fieldopt::ertwrapper
        PUBLIC fieldopt::reservoir)

target_include_directories(constraintmath PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/well_constraint_projections>)

add_compile_options(-std=c++11)

if (BUILD_TESTING)
    # Unit tests
    include_directories(${EIGEN3_INCLUDE_DIR})
	add_executable(test_constraintmath ${CONSTRAINTMATH_TESTS})
    target_link_libraries(test_constraintmath
            fieldopt::constraintmath
            ${gtest}
            ${gtest_main}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_constraintmath COMMAND $<TARGET_FILE:test_constraintmath>)
endif()

install( TARGETS constraintmath
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
