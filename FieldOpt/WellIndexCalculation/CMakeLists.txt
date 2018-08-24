cmake_minimum_required(VERSION 2.8)
project(wellindexcalculator)

# Dbg ==================================================================
message("\n============================================================")
message(STATUS "PROJECT_NAME => ${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Options ==============================================================
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(WIC_LIB_TARGET wellindexcalculator)
set(WIC_LIB_TARGET_ADG _wic-rixx)

# ResInsight ===========================================================
set(CVF_ENABLE_ASSERTS 1)
set(CVF_ENABLE_TIGHT_ASSERTS 1)
include(resinxx.cmake)

# Include: Qt ==========================================================
include_directories(${Qt5Core_INCLUDE_DIRS})

# Compile options ======================================================
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-std=c++11)


# Find libraries =======================================================
# Additional Ert libs (if necessary); ert_ecl and ert_util already
# defined in ERTWrapper

#    find_library(ert_eclxx
#            NAMES ecl
#            HINTS ${CMAKE_BINARY_DIR}/libraries/lib
#            REQUIRED)
#
#    find_library(ert_utilxx
#            NAMES ert_util
#            HINTS ${CMAKE_BINARY_DIR}/libraries/lib
#            REQUIRED)
#
#    find_library(ert_well
#            NAMES ecl_well
#            HINTS ${CMAKE_BINARY_DIR}/libraries/lib
#            REQUIRED)
#
#    find_library(ert_geometry
#            NAMES ert_geometry
#            HINTS ${CMAKE_BINARY_DIR}/libraries/lib
#            REQUIRED)

# Include dirs =========================================================
include_directories(
    resinxx/rixx_app_fwk
    resinxx/rixx_core_geom
    resinxx/rixx_res_mod
    resinxx/rixx_grid
)

message(".............................................................")
if (NOT BUILD_WIC_ADGPRS)

# controls ricasedata delete
add_definitions(-DFIELDOPT_BUILD)

add_library(${WIC_LIB_TARGET}
    intersected_cell.cpp
#    wellindexcalculator.cpp
    #
    wicalc_rixx.cpp
    ${RIXX_ALL_CPP_FILES}
    #
        WellDefinition.h)

add_library(fieldopt::${WIC_LIB_TARGET}
    ALIAS ${PROJECT_NAME})

message(STATUS "Set WIC_LIB_TARGET: ${WIC_LIB_TARGET}")


endif()

if (NOT BUILD_WIC_ADGPRS)

set_target_properties(${WIC_LIB_TARGET}
    PROPERTIES COMPILE_FLAGS "-fPIC")

target_link_libraries(${WIC_LIB_TARGET}
    PUBLIC fieldopt::reservoir
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC Qt5::Core
    PUBLIC ri:ert_ecl
    )

install(TARGETS wellindexcalculator
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

#add_executable(wicalc
#    main.cpp
#    intersected_cell.cpp
#    wellindexcalculator.cpp
#    #
#    wicalc_rixx.cpp
#    ${RIXX_ALL_CPP_FILES}
#    )
#
#target_link_libraries(wicalc
#    PUBLIC fieldopt::${WIC_LIB_TARGET}
#    PUBLIC ${Boost_LIBRARIES}
#    PUBLIC Qt5::Core
#    #
#    PUBLIC ${ERTLIB}
#    PUBLIC ${ert_util}
#    )
#
#install(TARGETS wicalc wellindexcalculator
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib/static
#    )

endif()


# Testing ==============================================================
#if (BUILD_TESTING AND NOT BUILD_WIC_ADGPRS)
#
#  # Unit tests
#  find_package(GTest REQUIRED)
#
#  include_directories(${GTEST_INCLUDE_DIRS}
#      ${EIGEN3_INCLUDE_DIR} tests)
#
#  add_executable(test_wellindexcalculator
#      tests/test_intersected_cells.cpp
#      tests/test_single_cell_wellindex.cpp
#      tests/test_standalone_wicalc.cpp)
#
#  target_link_libraries(test_wellindexcalculator
#      fieldopt::wellindexcalculator
#      ${GTEST_BOTH_LIBRARIES}
#      ${CMAKE_THREAD_LIBS_INIT})
#
#  add_test(NAME test_wellindexcalculator
#      COMMAND $<TARGET_FILE:test_wellindexcalculator>)
#endif()


# Debug: include dirs ==================================================
include(../InclDirDbg.cmake)
