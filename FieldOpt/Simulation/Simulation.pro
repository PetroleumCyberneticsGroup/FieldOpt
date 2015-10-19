include(../defaults.pri)

CONFIG   -= app_bundle
CONFIG += c++11

LIBS += -L../Model -lmodel

TEMPLATE = lib

TARGET = simulation

HEADERS += \
    simulators/simulator.h \
    simulators/eclsimulator.h

SOURCES += \
    simulators/eclsimulator.cpp \
    simulators/simulator.cpp
