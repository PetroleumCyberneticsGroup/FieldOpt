project( adgprsresultsreader )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
set ( adgprsresultsreader_HDRS
	json_summary_reader.h
	adgprs_results_reader.h
	)

set ( adgprsresultsreader_SRCS
	json_summary_reader.cpp
	adgprs_results_reader.cpp
	)

add_library ( adgprsresultsreader SHARED ${adgprsresultsreader_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( adgprsresultsreader  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
