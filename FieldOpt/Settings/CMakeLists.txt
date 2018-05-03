cmake_minimum_required(VERSION 2.8)
project(settings)

# Dbg ======================================================
message("\n============================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Add library ==============================================
add_library(settings
    settings_exceptions.h
    optimizer.cpp
    optimization.cpp
    simulator.cpp
    model.cpp
    settings.cpp
    deck_parser.cpp)

add_library(fieldopt::settings
    ALIAS ${PROJECT_NAME})

# Target link libraries ====================================
target_link_libraries(settings
    PUBLIC ${ecl_LIBRARIES}
    PUBLIC ${opm-common_LIBRARIES}
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC Qt5::Core)

# Target include directories ===============================
target_include_directories(settings PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Compile options ==========================================
add_compile_options(-std=c++11)

# Unit tests ===============================================
if (BUILD_TESTING)

  # Unit tests
  add_executable(test_settings
      tests/test_settings.cpp
      tests/test_settings_model.cpp
      tests/test_settings_optimizer.cpp
      tests/test_settings_simulator.cpp
      tests/test_deck_parser.cpp
      tests/test_schedule_import.cpp)

  target_link_libraries(test_settings
      fieldopt::settings
      ${Boost_LIBRARIES}
      ${gtest}
      ${gtest_main}
      ${CMAKE_THREAD_LIBS_INIT})

  add_test(NAME test_settings COMMAND $<TARGET_FILE:test_settings>)
endif()

# Install targets ==========================================
install( TARGETS settings
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

# Debug: include dirs ======================================
include(../InclDirDbg.cmake)