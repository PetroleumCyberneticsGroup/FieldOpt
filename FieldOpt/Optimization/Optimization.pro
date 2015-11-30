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
    optimization_exceptions.h

SOURCES += \
    objective/objective.cpp \
    objective/weightedsum.cpp \
    case.cpp
