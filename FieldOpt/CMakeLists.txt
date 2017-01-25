cmake_minimum_required(VERSION 3.2)
project(FieldOpt VERSION 0.3 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options =======================================================================================
option( BUILD_TESTING     "Build unit tests"                                               ON )
option( COPY_EXAMPLES     "Copy examples to build directory (needed for many unit tests)"  ON )
option( BUILD_WIC_ONLY    "Only build the WellIndexCalculator and its dependencies"        OFF )
option( BUILD_WIC_ADGPRS  "Build WellIndexCalculator as ADGPRS plugin (WIN)"               OFF )

# Local bin =====================================================================================
if (BUILD_TESTING AND NOT COPY_EXAMPLES)
    message("It is recommended to copy examples when building texts.
    Some of the tests use files in the examples directory.")
endif()

# Qt libraries ==================================================================================
if (NOT BUILD_WIC_ONLY)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    find_package(Qt5Core REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})
    find_package(HDF5 REQUIRED COMPONENTS CXX)
    add_definitions(${HDF5_DEFINITIONS})
    include_directories(${HDF5_INCLUDE_DIRS})
    find_package(Threads REQUIRED)
endif()

# Boost libraries ===============================================================================
find_package(Boost REQUIRED) # 1.63
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multithreading

if (BUILD_WIC_ADGPRS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

    set(Boost_USE_STATIC_LIBS    ON)  # enable static linking
    set(Boost_USE_MULTITHREADED  OFF) # enable multithreading

    set(Boost_USE_STATIC_RUNTIME OFF) # enable static linking to c++ runtime
    set(Boost_THREADAPI win32)
    find_package(Boost COMPONENTS REQUIRED program_options)
else()
    find_package(Boost COMPONENTS REQUIRED program_options serialization mpi system filesystem)
endif()

# Eigen =========================================================================================
find_package(Eigen3 REQUIRED)

# Set this directory as the root dir
include_directories(${CMAKE_SOURCE_DIR})

# Testing =======================================================================================
if (BUILD_TESTING)
    include(CTest)
    enable_testing()
endif()

add_subdirectory(ERTWrapper)
add_subdirectory(Reservoir)
add_subdirectory(FieldOpt-WellIndexCalculator)

if (NOT BUILD_WIC_ONLY)
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
    file(COPY ${EXECUTION_SCRIPTS} DESTINATION ${CMAKE_BINARY_DIR}/execution_scripts)

    ## Copy the test driver file into the build dir
    file(COPY ${CMAKE_SOURCE_DIR}/Settings/tests/driver/driver.json DESTINATION ${CMAKE_BINARY_DIR}/examples)
    file(COPY ${CMAKE_SOURCE_DIR}/Settings/tests/driver/driver_sane.json DESTINATION ${CMAKE_BINARY_DIR}/examples)

    ## Create an empty directory for test output
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldopt_output)
endif ()

if (COPY_EXAMPLES)
    ## Copy the examples into the build dir
    file(COPY ${CMAKE_SOURCE_DIR}/../examples DESTINATION ${CMAKE_BINARY_DIR}/)
endif()
