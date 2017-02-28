cmake_minimum_required(VERSION 3.2)
project(FieldOpt VERSION 0.3 LANGUAGES CXX)

# Build dirs ===========================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install dirs =========================================================
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(BUNDLE_NAME FieldOptBundle)
set(BUNDLE_PATH "${CMAKE_INSTALL_PREFIX}/${BUNDLE_NAME}")
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH which point
# to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")
# END Install dirs =====================================================

# Options 1: Unit tests, example dir ===================================
option(BUILD_TESTING "Build unit tests"                 ON)
option(COPY_EXAMPLES "Copy examples to build directory" ON)

if (BUILD_TESTING AND NOT COPY_EXAMPLES)
    message("It is recommended to copy examples when building unit
    tests. Several of the tests use files in the examples directory.")
endif()

# Options 2: Compile WIC sources only ==================================
# NOTE: only one of these options should be ON
# Build standalone well index calculator executable
option(BUILD_WIC_ONLY "Build well index calculator exe" OFF)
# Build OS-dependent well index calculator (shared) library:
option(BUILD_WIC_ADGPRS "Build WIC as ADGPRS plugin"    OFF)

# Qt libraries =========================================================
if (NOT BUILD_WIC_ONLY)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    find_package(Qt5Core REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    find_package(HDF5 REQUIRED COMPONENTS CXX)
    add_definitions(${HDF5_DEFINITIONS})
    include_directories(${HDF5_INCLUDE_DIRS})
    find_package(Threads REQUIRED)
endif()

# Boost libraries: default-settings ====================================
find_package(Boost REQUIRED) # 1.63
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multithreading

# Boost libraries: option-dependent settings ===========================
if (BUILD_WIC_ADGPRS)  # Implementation of BUILD_WIC_ADGPRS option

    # CMAKE_EXE_LINKER_FLAGS --> TODO: Double-check if absolutely necessary
    set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

    # CMAKE_SHARED_LINKER_FLAGS --> TODO: Double-check if absolutely necessary
    set(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")

    # This is opposite of default settings --> TODO: Double-check if necessary
    set(Boost_USE_MULTITHREADED  OFF) # Enable multithreading

    # Linking options
    set(Boost_USE_STATIC_RUNTIME OFF) # Enable static linking to c++ runtime
    set(Boost_THREADAPI win32) # Only needed for WIN32 option?

    # Find packages
    find_package(Boost COMPONENTS
            REQUIRED program_options system filesystem)

elseif(BUILD_WIC_ONLY)  # Building WIC executable with dependencies only
    find_package(Boost COMPONENTS
            REQUIRED program_options system filesystem)

else() # Building all FieldOpt sources
    find_package(Boost COMPONENTS
            REQUIRED program_options serialization mpi system filesystem random)
endif()

# ERT Include dir, primarily for use on cluster (Maur). This assumes that
#  headers and libs are placed inside a 'libraries' directory in the build
#  directory.
include_directories(${CMAKE_BINARY_DIR}/libraries/include/)

# Boost libraries: option-dependent settings ===========================
# CMAKE_MODULE_PATH: Commands like include() and find_package() search
# for files in directories listed by this variable before checking the
# default modules that come with CMake. (Added by AGChitu)
# --> TODO: Check if really needed. Remove if not.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# Eigen ================================================================
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Set this directory as the root dir
include_directories(${CMAKE_SOURCE_DIR})

# Testing ==============================================================
if (BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

# Compile FieldOpt sources =============================================
add_subdirectory(ERTWrapper)
add_subdirectory(Reservoir)
add_subdirectory(FieldOpt-WellIndexCalculator)

if (NOT BUILD_WIC_ONLY AND NOT BUILD_WIC_ADGPRS)
    add_subdirectory(Utilities)
    add_subdirectory(Settings)
    add_subdirectory(Model)
    add_subdirectory(Optimization)
    add_subdirectory(Simulation)
    add_subdirectory(Runner)
    add_subdirectory(ConstraintMath)
    add_subdirectory(Hdf5SummaryReader)

    # Copy execution scripts
    file(GLOB EXECUTION_SCRIPTS
            "Simulation/execution_scripts/*.sh")
    file(COPY ${EXECUTION_SCRIPTS}
            DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/execution_scripts)

    ## Copy the test driver file into the build dir
    file(COPY ${CMAKE_SOURCE_DIR}/Settings/tests/driver/driver.json
            DESTINATION ${CMAKE_BINARY_DIR}/examples)
    file(COPY ${CMAKE_SOURCE_DIR}/Settings/tests/driver/driver_sane.json
            DESTINATION ${CMAKE_BINARY_DIR}/examples)

    ## Copy wic benchmark wells into the build dir
    file(COPY ${CMAKE_SOURCE_DIR}/FieldOpt-WellIndexCalculator/tests/well_index_benchmark/wic-benchmark-wells
            DESTINATION ${CMAKE_BINARY_DIR}/)

    ## Create an empty directory for test output
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldopt-output)

endif ()

# Copy example files ===================================================
if (COPY_EXAMPLES)
    ## Copy the examples into the build dir
    file(COPY ${CMAKE_SOURCE_DIR}/../examples
            DESTINATION ${CMAKE_BINARY_DIR}/)
endif()
