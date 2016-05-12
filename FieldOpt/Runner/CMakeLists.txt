cmake_minimum_required( VERSION 3.2 )
project(FieldOpt LANGUAGES CXX)
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
set ( FieldOpt_HDRS
	runtime_settings.h
	runners/main_runner.h
	runners/abstract_runner.h
	runners/serial_runner.h
	logger.h
	bookkeeper.h
	)

set ( FieldOpt_SRCS
	main.cpp
	runtime_settings.cpp
	runners/main_runner.cpp
	runners/abstract_runner.cpp
	runners/serial_runner.cpp
	logger.cpp
	bookkeeper.cpp
	)

add_executable ( FieldOpt ${FieldOpt_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( FieldOpt  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
