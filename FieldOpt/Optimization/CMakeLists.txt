project( optimization )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
set ( optimization_HDRS
	objective/objective.h
	objective/weightedsum.h
	case.h
	optimization_exceptions.h
	case_handler.h
	constraints/constraint.h
	constraints/box_constraint.h
	constraints/constraint_handler.h
	optimizer.h
	optimizers/compass_search.h
	constraints/well_spline_length.h
	constraints/interwell_distance.h
	constraints/well_spline_constraint.h
	constraints/combined_spline_length_interwell_distance.h
	)

set ( optimization_SRCS
	objective/objective.cpp
	objective/weightedsum.cpp
	case.cpp
	case_handler.cpp
	constraints/constraint.cpp
	constraints/box_constraint.cpp
	constraints/constraint_handler.cpp
	optimizer.cpp
	optimizers/compass_search.cpp
	constraints/well_spline_length.cpp
	constraints/interwell_distance.cpp
	constraints/well_spline_constraint.cpp
	constraints/combined_spline_length_interwell_distance.cpp
	)

add_library ( optimization SHARED ${optimization_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( optimization  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
