
# Debug: include dirs ==================================================
message(".............................................................")
message(STATUS "List current include dirs:")
get_property(DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY INCLUDE_DIRECTORIES)
foreach(DIR ${DIRS})
    message(STATUS "DIR='${DIR}'")
endforeach()
message(STATUS "END PROJECT (${PROJECT_NAME})")
message("============================================================\n")