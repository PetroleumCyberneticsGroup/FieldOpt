cmake_minimum_required(VERSION 2.8)
project(FieldOpt)

# General config =======================================================
# CMAKE_MODULE_PATH: Commands like include() and find_package() search
# for files in directories listed by this variable before checking the
# default modules that come with CMake.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(GetSysInfo.cmake)
include(MaurConfig.cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -Wformat")

# Compile options ======================================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ResInsight ===========================================================
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DCVF_LINUX)
  message(STATUS "CMAKE_SYSTEM_NAME => ${CMAKE_SYSTEM_NAME}")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DCVF_LINUX -pipe -Wextra ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -g3 -O0 -DDEBUG -D_DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNO_DEBUG")
endif()

# Build dirs ===========================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable unit tests + example dir ======================================
option(BUILD_TESTING "Build unit tests"                 OFF)
option(COPY_EXAMPLES "Copy examples to build directory" ON)

if (BUILD_TESTING AND NOT COPY_EXAMPLES)
  message("It is recommended to copy examples when building unit
    tests. Several of the tests use files in the examples directory.")
endif()

# Enable compilation of WIC as standalone executable and/or lib ========
# NOTE: only one of the following two options should be ON at same time
# [1] Build standalone well index calculator executable
option(BUILD_WIC_ONLY "Build well index calculator exe" OFF)
# [2] Build OS-dependent well index calculator (shared) library:
option(BUILD_WIC_ADGPRS "Build WIC as ADGPRS plugin"    OFF)

# Build executable for WI benchmarktests ===============================
option(BUILD_BENCHMARK "Build exec for wic RMS benchmark" OFF)

# Include Eigen + RpolyPlusPlus ========================================
if(NOT COMPILATION_AT_MAUR)
  find_package(Eigen3 REQUIRED)
  include_directories(${EIGEN3_INCLUDE_DIR})
  message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")

  find_library(RpolyPlusPlus
      NAMES rpoly_plus_plus
      HINTS ${CMAKE_BINARY_DIR}/libraries/lib
      REQUIRED)
endif()

# Custom ERT include dir (ResInsight) ==================================
# Placed early on so it overrides standard include dir (at /usr/include)
set(ERT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/resinsight/Ert/lib/include)
include_directories(${CMAKE_SOURCE_DIR} ${ERT_INCLUDE_DIR})

# Build WIC executable  and/or WIC ADGPRS lib ==========================
# Include HDF5 + Qt5 ===================================================
message("=============================================================")
if (BUILD_WIC_ONLY OR BUILD_WIC_ADGPRS)
  message(STATUS "Building BUILD_WIC_ONLY: ${BUILD_WIC_ONLY}")
  message(STATUS "Building BUILD_WIC_ADGPRS: ${BUILD_WIC_ADGPRS}")
  message(STATUS "Skipping: Qt5Core and HDF5")
else()
  message(STATUS "Building FIELDOPT. Loading: Qt5Core and HDF5")
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  find_package(HDF5 REQUIRED COMPONENTS CXX)
  add_definitions(${HDF5_DEFINITIONS})
  include_directories(${HDF5_INCLUDE_DIRS})
  find_package(Threads REQUIRED)

  find_package(Qt5Core REQUIRED)
  message(STATUS "Qt5Core_QTMAIN_LIBRARIES: ${Qt5Core_QTMAIN_LIBRARIES}")
  message(STATUS "Qt5Core_INCLUDE_DIRS: ${Qt5Core_INCLUDE_DIRS}")
  include_directories(${Qt5Core_INCLUDE_DIRS})
endif()

# Boost libraries: default-settings ====================================
message("=============================================================")
find_package(Boost REQUIRED) # 1.63
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
endif()
set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multithreading

# Boost libraries: option-dependent settings ===========================
if (BUILD_WIC_ADGPRS)  # Implementation of BUILD_WIC_ADGPRS option

  set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking

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

# Pring debug info: include dirs + source dirs =========================
include(DebugMsg.cmake)

# TO BE REMOVED
#include_directories(${CMAKE_BINARY_DIR}/libraries/include/)

# Enable testing =======================================================
if (BUILD_TESTING)
  include(CTest)
  enable_testing()
endif()

# Compile FieldOpt sources =============================================
add_subdirectory(ERTWrapper)
add_subdirectory(Reservoir)
add_subdirectory(FieldOpt-WellIndexCalculator)

if (NOT (BUILD_WIC_ONLY OR BUILD_WIC_ADGPRS))
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

  if(BUILD_BENCHMARK)
    ## Copy wic benchmark wells into the build dir
    set(WIC_DIR "${CMAKE_SOURCE_DIR}/FieldOpt-WellIndexCalculator")
    file(COPY "${WIC_DIR}/tests/well_index_benchmark/wic-benchmark-wells"
        DESTINATION ${CMAKE_BINARY_DIR}/)
  endif()

  ## Create an empty directory for test output
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldopt-output)
endif()

# Compile options ======================================================
add_compile_options(-std=fPIC)

# Copy example files ===================================================
if (COPY_EXAMPLES)
  ## Copy the examples into the build dir
  file(COPY ${CMAKE_SOURCE_DIR}/../examples
      DESTINATION ${CMAKE_BINARY_DIR}/)
endif()
