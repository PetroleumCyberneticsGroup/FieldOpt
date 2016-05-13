cmake_minimum_required(VERSION 3.2)
project(optimization)

add_library(optimization
        optimization_exceptions.h
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
		constraints/combined_spline_length_interwell_distance.cpp)
add_library(fieldopt::optimization ALIAS ${PROJECT_NAME})

target_link_libraries(optimization
        PUBLIC fieldopt::model
        PUBLIC fieldopt::simulation
        PUBLIC fieldopt::wellindexcalculator
        PUBLIC RPolyPlusPlus)

target_include_directories(optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/constraints>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/objective>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/optimizers>)

target_compile_features(ertwrapper
        PUBLIC cxx_auto_type
        PUBLIC cxx_range_for)


#include(../defaults.pri)
#
#
#CONFIG   -= app_bundle
#CONFIG += c++11
#
#LIBS += -L$$OUT_PWD/../Model -lmodel
#LIBS += -L$$OUT_PWD/../Simulation -lsimulation
#LIBS += -L$$OUT_PWD/../WellIndexCalculator -lWellIndexCalculator
#
#LIBS += -lrpoly_plus_plus
#
#TEMPLATE = lib
#
#TARGET = optimization
#
#HEADERS += \
#objective/objective.h \
#objective/weightedsum.h \
#case.h \
#optimization_exceptions.h \
#case_handler.h \
#constraints/constraint.h \
#constraints/box_constraint.h \
#constraints/constraint_handler.h \
#optimizer.h \
#optimizers/compass_search.h \
#constraints/well_spline_length.h \
#constraints/interwell_distance.h \
#constraints/well_spline_constraint.h \
#constraints/combined_spline_length_interwell_distance.h
#
#SOURCES += \
#objective/objective.cpp \
#objective/weightedsum.cpp \
#case.cpp \
#case_handler.cpp \
#constraints/constraint.cpp \
#constraints/box_constraint.cpp \
#constraints/constraint_handler.cpp \
#optimizer.cpp \
#optimizers/compass_search.cpp \
#constraints/well_spline_length.cpp \
#constraints/interwell_distance.cpp \
#constraints/well_spline_constraint.cpp \
#constraints/combined_spline_length_interwell_distance.cpp
