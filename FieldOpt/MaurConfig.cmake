
# Compilation at Maur ==================================================
if(${HOST_HOSTNAME} MATCHES login.* OR ${HOST_HOSTNAME} MATCHES compute.*)
    option(COMPILATION_AT_MAUR " " ON)
else()
    option(COMPILATION_AT_MAUR " " OFF)
endif()

# Maur =================================================================
message("=============================================================")
if(COMPILATION_AT_MAUR)

    # RPOLYPOLY
    set(RPOLYPOLY_LIB $ENV{HOME}/git/libraries/RpolyPlusPlus/cmake-build-debug/lib)
    set(RpolyPlusPlus ${RPOLYPOLY_LIB}/librpoly_plus_plus.so)
    set(RPOLYPOLY_INCLUDE_DIR $ENV{HOME}/git/libraries/RpolyPlusPlus/include)
    include_directories(${RPOLYPOLY_INCLUDE_DIR})
    message(STATUS "RPOLYPOLY_LIB: ${RPOLYPOLY_LIB}")
    message(STATUS "RpolyPlusPlus: ${RpolyPlusPlus}")
    message(STATUS "RPOLYPOLY_LIB: ${RPOLYPOLY_INCLUDE_DIR}")

    # QT
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/git/libraries/qt5.5.1")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

    # EIGEN
    set(EIGEN3_INCLUDE_DIR $ENV{HOME}/git/libraries/eigen3.3-beta1-2/Eigen)
    include_directories($ENV{HOME}/git/libraries/eigen3.3-beta1-2)
    include_directories(${EIGEN3_INCLUDE_DIR})
    message(STATUS "EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
    message(STATUS "EIGEN VERSION: ${EIGEN_VERSION}")

    # ERT
    set(ERT_LIB $ENV{HOME}/git/libraries/ert/lib64)
    set(ERT_INCL_DIR $ENV{HOME}/git/libraries/ert/include)
    set(ert_ecl $ENV{HOME}/git/libraries/ert/lib64/libecl.so)
    set(ert_util $ENV{HOME}/git/libraries/ert/lib64/libert_util.so)
    include_directories(${ERT_INCL_DIR})
    include_directories(${ERT_INCL_DIR}/ert/ecl)
    include_directories(${ERT_INCL_DIR}/ert/ecl_well)
    include_directories(${ERT_INCL_DIR}/ert/geometry)
    include_directories(${ERT_INCL_DIR}/ert/util)
    message(STATUS "ERT_LIB: ${ERT_LIB}")
    message(STATUS "ERT_INCL_DIR: ${ERT_INCL_DIR}")
    message(STATUS "ert_ecl: ${ert_ecl}")
    message(STATUS "ert_util: ${ert_util}")
endif()