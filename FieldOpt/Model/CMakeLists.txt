cmake_minimum_required(VERSION 2.8)
project(model)

# Dbg ==================================================================
message("\n============================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Add library ==========================================================
add_library(model
        model.cpp
        wells/well.cpp
        wells/well_exceptions.h
        wells/wellbore/completions/completion.cpp
        wells/wellbore/completions/perforation.cpp
        wells/wellbore/trajectory.cpp
        wells/wellbore/wellspline.cpp
        wells/control.cpp
        wells/wellbore/wellblock.cpp
        properties/property.h
        properties/property_exceptions.h
        properties/binary_property.cpp
        properties/variable_property_container.cpp
        properties/continous_property.cpp
        properties/discrete_property.cpp
        properties/property.cpp
        model_synchronization_object.cpp
        wells/wellbore/pseudo_cont_vert.cpp)
add_library(fieldopt::model ALIAS ${PROJECT_NAME})

# Target link libraries ================================================
target_link_libraries (model
        PUBLIC fieldopt::reservoir
        PUBLIC fieldopt::wellindexcalculator
        PUBLIC fieldopt::settings
        PUBLIC fieldopt::constraintmath
        PUBLIC fieldopt::runner
        PUBLIC Qt5::Core
        PUBLIC ${Boost_LIBRARIES})

# Target include directories ===========================================
target_include_directories(model PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/properties>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wells/wellbore/completions>)

# Compile options ======================================================
add_compile_options(-std=c++11)

# Unit tests ===========================================================
if (BUILD_TESTING)
    # Unit tests
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS} tests)
    add_executable(test_model
            tests/test_model.cpp
            tests/variables/test_properties.cpp
            tests/variables/test_variable_property_container.cpp
            tests/wells/test_control.cpp
            tests/wells/test_perforation.cpp
            tests/wells/test_trajectory.cpp
            tests/wells/test_well.cpp
            tests/wells/test_wellblock.cpp
            tests/test_resource_variable_property_container.h
            tests/test_resource_model.h t
            ests/wells/test_pseudo_cont_vert.cpp)

    target_link_libraries(test_model
            fieldopt::model
            fieldopt::reservoir
            fieldopt::optimization
            ${GTEST_BOTH_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT})

    add_test(NAME test_model COMMAND $<TARGET_FILE:test_model>)
endif()

# Install targets ======================================================
install(TARGETS model
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )

# Debug: include dirs ==================================================
include(../InclDirDbg.cmake)
