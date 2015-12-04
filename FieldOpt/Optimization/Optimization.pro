include(../defaults.pri)


CONFIG   -= app_bundle
CONFIG += c++11

LIBS += -L../Model -lmodel

TEMPLATE = lib

TARGET = optimization

HEADERS += \
    objective/objective.h \
    objective/weightedsum.h \
    case.h \
    optimization_exceptions.h \
    case_handler.h \
    constraints/constraint.h \
    constraints/box_constraint.h \
    constraints/constraint_handler.h \
    optimizer.h

SOURCES += \
    objective/objective.cpp \
    objective/weightedsum.cpp \
    case.cpp \
    case_handler.cpp \
    constraints/constraint.cpp \
    constraints/box_constraint.cpp \
    constraints/constraint_handler.cpp \
    optimizer.cpp
