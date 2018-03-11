# Colors ===============================================================
#if(NOT WIN32)

#ccred=echo -e "\033[0;31m"
#ccred=$(shell echo -e "\033[0;31m")
#    string(ASCII 27 Esc)
#    set(AEND         "\\e[0m")
#    set(CBOLD        "${Esc}[1m")
#    set(BRED         "\\e[1;34m")
#    set(BGREEN       "${Esc}[32m")
#    set(BYELLOW      "${Esc}[33m")
#    set(BBLUE        "${Esc}[34m")
#    set(BMAGENTA     "${Esc}[35m")
#    set(BCYAN        "${Esc}[36m")
#    set(BWHITE       "${Esc}[37m")
#    set(BOLDBRED     "${Esc}[1;31m")
#    set(BOLDBGREEN   "${Esc}[1;32m")
#    set(BOLDBYELLOW  "${Esc}[1;33m")
#    set(BOLDBBLUE    "${Esc}[1;34m")
#    set(BOLDBMAGENTA "${Esc}[1;35m")
#    set(BOLDBCYAN    "${Esc}[1;36m")
#    set(BOLDBWHITE   "${Esc}[1;37m")
#endif()

# Debug: include dirs ==================================================
message(".............................................................")
message(STATUS "List current include dirs:")
get_property(DIRS DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(DIR ${DIRS})
    message(STATUS "DIR='${DIR}'")
endforeach()

# Debug: Source dir ====================================================
message(".............................................................")

message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")



