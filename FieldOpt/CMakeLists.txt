cmake_minimum_required(VERSION 2.8)
project(FieldOpt)

# ----------------------------------------------------------
# ╔═╗╔╦╗╔═╗╦╔═╔═╗
# ║  ║║║╠═╣╠╩╗║╣
# ╚═╝╩ ╩╩ ╩╩ ╩╚═╝
# ----------------------------------------------------------

# General config ===========================================
# CMAKE_MODULE_PATH: Commands like include() and find_package() search
# for files in directories listed by this variable before checking the
# default modules that come with CMake.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(GetSysInfo.cmake)
include(MaurConfig.cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -Wformat")

# Compile options ==========================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_COLOR_MAKEFILE ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

# Sanitizing ===============================================
option(USE_SANITIZING "Build sanitizing features for debugging"  OFF)
if (USE_SANITIZING)
#  https://github.com/google/sanitizers
#  Sanitizing is a feature built into more recent versions of GCC (>4.7).
#  It add additional (better) error traces for memory-related problems.
#  AddressSanitizer (detects addressability issues) and LeakSanitizer (detects memory leaks) [address] [leak]
#  ThreadSanitizer (detects data races and deadlocks) for C++ and Go                         [thread]
#  MemorySanitizer (detects use of uninitialized memory)                                     [memory]
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fsanitize=leak -g")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address  -fsanitize=leak -g")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address  -fsanitize=leak -g")
  set (CMAKE_MODULE_LINKER_FLAGS  "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address  -fsanitize=leak -g")
endif()

#CMAKE_C_FLAGS:STRING=-fsanitize=address  -fsanitize=leak -g
#CMAKE_EXE_LINKER_FLAGS:STRING=-fsanitize=address  -fsanitize=leak
#CMAKE_MODULE_LINKER_FLAGS:STRING=-fsanitize=address  -fsanitize=leak


# Build dirs ===============================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include dirs =============================================
include_directories(${PROJECT_SOURCE_DIR})

# ----------------------------------------------------------
# ╦═╗╔═╗╔═╗╦╔╗╔╔═╗╦╔═╗╦ ╦╔╦╗
# ╠╦╝║╣ ╚═╗║║║║╚═╗║║ ╦╠═╣ ║
# ╩╚═╚═╝╚═╝╩╝╚╝╚═╝╩╚═╝╩ ╩ ╩
# ----------------------------------------------------------

# ResInsight ===============================================
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

# ----------------------------------------------------------
# ╔═╗╦═╗╔╦╗   ╔═╗╔═╗╦
# ║╣ ╠╦╝ ║    ║╣ ║  ║
# ╚═╝╩╚═ ╩────╚═╝╚═╝╩═╝
# ----------------------------------------------------------

add_subdirectory(ThirdParty/libecl)


# ----------------------------------------------------------
# ╦  ╔═╗╔═╗╔═╗╦    ╦  ╦╔╗ ╔═╗
# ║  ║ ║║  ╠═╣║    ║  ║╠╩╗╚═╗
# ╩═╝╚═╝╚═╝╩ ╩╩═╝  ╩═╝╩╚═╝╚═╝
# ----------------------------------------------------------

# Set bundled dependencies first in package search path
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib/cmake)
message(STATUS "CMAKE_PREFIX_PATH:" ${CMAKE_PREFIX_PATH})

# Add local third party include dirs
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/libraries/include)

# ----------------------------------------------------------
# ╦ ╦╔╗╔╦╔╦╗  ╔╦╗╔═╗╔═╗╔╦╗
# ║ ║║║║║ ║    ║ ║╣ ╚═╗ ║
# ╚═╝╝╚╝╩ ╩    ╩ ╚═╝╚═╝ ╩
# ----------------------------------------------------------

# Enable unit tests + example dir ==========================
option(BUILD_TESTING "Build unit tests"                 ON)
option(COPY_EXAMPLES "Copy examples to build directory" ON)

if (BUILD_TESTING AND NOT COPY_EXAMPLES)
  message("It is recommended to copy examples when building unit
    tests. Several of the tests use files in the examples directory.")
endif()

# ----------------------------------------------------------
# ╔═╗╔╦╗╔═╗╔═╗╦═╗╔═╗  ╦  ╦╔╗
# ╠═╣ ║║║ ╦╠═╝╠╦╝╚═╗  ║  ║╠╩╗
# ╩ ╩═╩╝╚═╝╩  ╩╚═╚═╝  ╩═╝╩╚═╝
# ----------------------------------------------------------

# Enable compilation of WIC as standalone executable and/or lib ========
# NOTE: only one of the following two options should be ON at same time
# [1] Build standalone well index calculator executable
option(BUILD_WIC_ONLY "Build well index calculator exe" OFF)
# [2] Build OS-dependent well index calculator (shared) library:
option(BUILD_WIC_ADGPRS "Build WIC as SHARED lib"       OFF)

# ----------------------------------------------------------
# ╔╦╗╔═╗╦ ╦╦═╗
# ║║║╠═╣║ ║╠╦╝
# ╩ ╩╩ ╩╚═╝╩╚═
# ----------------------------------------------------------

# Build executable for WI benchmarktests ===================
option(BUILD_BENCHMARK "Build exec for wic RMS benchmark" OFF)

# ----------------------------------------------------------
# ╔═╗╦╔═╗╔═╗╔╗╔
# ║╣ ║║ ╦║╣ ║║║
# ╚═╝╩╚═╝╚═╝╝╚╝
# ----------------------------------------------------------

# Include Eigen: Options ===================================
if(NOT COMPILATION_AT_MAUR)
  find_package(Eigen3 REQUIRED)
  include_directories(${EIGEN3_INCLUDE_DIR})
  message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
endif()

if(NOT EIGEN3_FOUND)
  message(STATUS "Using local Eigen3")
  set(EIGEN3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ThirdParty/Eigen3)
#  set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/ThirdParty/Eigen3)
endif()

# ----------------------------------------------------------
# ╔═╗╔═╗
# ║ ╦╠═╝
# ╚═╝╩
# ╦ ╦╔╦╗╔═╗
# ╠═╣ ║║╠╣
# ╩ ╩═╩╝╚
# ----------------------------------------------------------

# Build WIC executable  and/or WIC ADGPRS lib ==============
# Include HDF5 + Qt5 + GP + ECL ============================
message("==================================================")
if (BUILD_WIC_ONLY OR BUILD_WIC_ADGPRS)
  message(STATUS "Building BUILD_WIC_ONLY: ${BUILD_WIC_ONLY}")
  message(STATUS "Building BUILD_WIC_ADGPRS: ${BUILD_WIC_ADGPRS}")
  message(STATUS "Skipping: HDF5")
else()
  message(STATUS "Building FIELDOPT. Loading: HDF5")
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  find_package(HDF5 REQUIRED COMPONENTS CXX)
  add_definitions(${HDF5_DEFINITIONS})
  include_directories(${HDF5_INCLUDE_DIRS})
  find_package(Threads REQUIRED)

  message(".............................................................")
  message(STATUS "Loading potentially local libraries: gp + opm-common")

  find_library(gp NAMES gp
      HINTS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
      REQUIRED)

  find_package(opm-common
      PATHS
      ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
      ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/share/cmake/opm-common
      NO_DEFAULT_PATH
      REQUIRED)

  message(STATUS "ecl_LIBRARIES: ${ecl_LIBRARIES}")
  message(STATUS "ecl_LIBRARY: ${ecl_LIBRARY}")
  message(STATUS "ecl_FOUND: ${ecl_FOUND}")
  message(STATUS "gp: ${gp}")
  message(STATUS "opm-common: ${opm-common_LIBRARIES}")

endif()

# ----------------------------------------------------------
# ╔═╗╔╦╗
# ║═╬╗║
# ╚═╝╚╩
# ----------------------------------------------------------
message(".............................................................")
message(STATUS "Loading potentially local libraries: Qt")
find_package(Qt5Core
    HINTS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
    REQUIRED)

message(STATUS "Qt5Core_QTMAIN_LIBRARIES: ${Qt5Core_QTMAIN_LIBRARIES}")
message(STATUS "Qt5Core_INCLUDE_DIRS: ${Qt5Core_INCLUDE_DIRS}")
include_directories(${Qt5Core_INCLUDE_DIRS})

# ----------------------------------------------------------
# ╦═╗╔═╗╔═╗╦ ╦ ╦╔═╗╔═╗╦ ╦ ╦
# ╠╦╝╠═╝║ ║║ ╚╦╝╠═╝║ ║║ ╚╦╝
# ╩╚═╩  ╚═╝╩═╝╩ ╩  ╚═╝╩═╝╩
# ----------------------------------------------------------

# Include RpolyPlusPlus: Options ===========================
message("..................................................")

option(USE_RPOLY_SYSTEM "" OFF)
option(USE_RPOLY_LOCAL "" ON)

if(USE_RPOLY_SYSTEM)

  #  find_library(RpolyPlusPlus
  #      NAMES rpoly_plus_plus
  #      NO_CMAKE_PATH
  #      REQUIRED)

  #  set(RpolyPlusPlus /usr/lib/librpoly_plus_plus.so)
  #  set(RpolyPlusPlus_LIBRARY "${RpolyPlusPlus}")
  #  set(RpolyPlusPlus_LIBRARIES "${RpolyPlusPlus}")

  #  set(RpolyPlusPlus_INCLUDE_DIR /usr/include/RpolyPlusPlus)
  #  include_directories(${CMAKE_SOURCE_DIR} ${RpolyPlusPlus_INCLUDE_DIR})

elseif(USE_RPOLY_LOCAL)

  # NOT WORKING:
  #  find_library(RpolyPlusPlus
  #      NAMES rpoly_plus_plus
  #      NO_DEFAULT_PATH
  #      REQUIRED)

  # Lib
  find_library(RpolyPlusPlus_LIBRARY
      NAMES rpoly_plus_plus
      PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/lib
      NO_DEFAULT_PATH
      REQUIRED)

  # Include
  find_path(RpolyPlusPlus_INCLUDE_DIR
      NAMES polynomial.h find_polynomial_roots_jenkins_traub.h
      PATHS ${CMAKE_SOURCE_DIR}/ThirdParty/libraries/include/RpolyPlusPlus
      NO_DEFAULT_PATH
      REQUIRED)

  set(RpolyPlusPlus_LIBRARIES ${RpolyPlusPlus_LIBRARY})

endif()

# ----------------------------------------------------------
# ╔╗ ╔═╗╔═╗╔═╗╔╦╗
# ╠╩╗║ ║║ ║╚═╗ ║
# ╚═╝╚═╝╚═╝╚═╝ ╩
# ----------------------------------------------------------

# Boost libraries: default-settings ========================
message("..................................................")
find_package(Boost REQUIRED) # 1.63
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
endif()
set(Boost_USE_STATIC_LIBS OFF) # enable dynamic linking
set(Boost_USE_MULTITHREAD ON)  # enable multithreading

# Boost libraries: option-dependent settings ===============
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
      REQUIRED regex program_options serialization mpi system filesystem random)
endif()

# ----------------------------------------------------------
# ╔╦╗╔╗ ╔═╗
#  ║║╠╩╗║ ╦
# ═╩╝╚═╝╚═╝
# ----------------------------------------------------------

# Pring debug info: include dirs + source dirs =============
include(DebugMsg.cmake)

## ADDED
## Set this directory as the root dir
include_directories(${CMAKE_SOURCE_DIR} ${ERT_INCLUDE_DIR})

# ----------------------------------------------------------
# ╔═╗╔╗╔╔═╗╔═╗╔╦╗
# ╚═╗║║║║ ║╠═╝ ║
# ╚═╝╝╚╝╚═╝╩   ╩
# ----------------------------------------------------------

## SNOPT ===================================================
#include_directories(${CMAKE_SOURCE_DIR}/FieldOpt-3rdPartySolvers/include/snopt)
#include_directories(${CMAKE_SOURCE_DIR}/FieldOpt-3rdPartySolvers/handlers)

# ----------------------------------------------------------
# ╦ ╦╔╗╔╦╔╦╗  ╔╦╗╔═╗╔═╗╔╦╗╔═╗
# ║ ║║║║║ ║    ║ ║╣ ╚═╗ ║ ╚═╗
# ╚═╝╝╚╝╩ ╩    ╩ ╚═╝╚═╝ ╩ ╚═╝
# ----------------------------------------------------------

# Enable testing ===========================================
if (BUILD_TESTING)
  include(CTest)
  enable_testing()
  find_library(gtest NAMES gtest HINTS ${PROJECT_SOURCE_DIR}/ThirdParty/libraries/lib REQUIRED)
  find_library(gtest_main NAMES gtest_main HINTS ${PROJECT_SOURCE_DIR}/ThirdParty/libraries/lib REQUIRED)
  set(GTEST_BOTH_LIBRARIES ${gtest} ${gtest_main})
  message(STATUS "gtest:      ${gtest}")
  message(STATUS "gtest_main: ${gtest_main}")
  message(STATUS "GTEST_BOTH_LIBRARIES: ${GTEST_BOTH_LIBRARIES}")

endif()

message(STATUS "END MAIN PROJECT (${PROJECT_NAME}) ==> STARTING SUBDIRS")
message("============================================================\n")

# Compile FieldOpt sources =================================
add_subdirectory(ERTWrapper)
add_subdirectory(Reservoir)
add_subdirectory(WellIndexCalculation)

if (NOT (BUILD_WIC_ONLY OR BUILD_WIC_ADGPRS))
  add_subdirectory(Utilities)
  add_subdirectory(Settings)
  add_subdirectory(Model)
  add_subdirectory(Optimization)
  add_subdirectory(Simulation)
  add_subdirectory(Runner)
  add_subdirectory(ConstraintMath)
  add_subdirectory(Hdf5SummaryReader)
#  add_subdirectory(FieldOpt-3rdPartySolvers)

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

  file(COPY ${CMAKE_SOURCE_DIR}/Settings/tests/driver/driver_hybridopt.json
          DESTINATION ${CMAKE_BINARY_DIR}/examples)

  ## Create an empty directory for test output
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldopt-output)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fieldopt-output/norne_test)
endif()

# Compile options ==========================================
add_compile_options(-std=fPIC)

# Copy example files =======================================
if (COPY_EXAMPLES)
  ## Copy the examples into the build dir
  file(COPY ${CMAKE_SOURCE_DIR}/../examples
      DESTINATION ${CMAKE_BINARY_DIR}/)
endif()

# ==========================================================
message("\n*************************************************************")
message("**** Set ERT_INCLUDE_DIR: ${ERT_INCLUDE_DIR}")
message("**** Set ert_util: ${ert_util}")
message("**** Set ERTLIB: ${ecl}")
message("*************************************************************\n")

message("\n*************************************************************")
message(STATUS "RpolyPlusPlus: ${RpolyPlusPlus}")
message(STATUS "RpolyPlusPlus_LIBRARY: ${RpolyPlusPlus_LIBRARY}")
message(STATUS "RpolyPlusPlus_LIBRARIES: ${RpolyPlusPlus_LIBRARIES}")
message(STATUS "RpolyPlusPlus_INCLUDE_DIR: ${RpolyPlusPlus_INCLUDE_DIR}")
message("*************************************************************\n")
