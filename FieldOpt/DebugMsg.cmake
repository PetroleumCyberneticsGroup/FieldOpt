
# Debug: include dirs ==================================================
message("=============================================================")
message(STATUS "List current include dirs:")
get_property(DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(DIR ${DIRS})
    message(STATUS "DIR='${DIR}'")
endforeach()

# Debug: Source dir ====================================================
message("=============================================================")
message(STATUS "CMAKE_MODULE_PATH: ${PROJECT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})


