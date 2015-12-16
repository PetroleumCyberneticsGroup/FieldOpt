include(../defaults.pri)

QT += core
QT -= gui

TARGET = FieldOpt
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

CONFIG += c++11
LIBS += -L../Model -lmodel
LIBS += -L../Optimization -loptimization
LIBS += -L../Simulation -lsimulation
LIBS += -L../Utilities -lutilities
LIBS += -lpthread -lgtest -pthread
LIBS += -lboost_program_options

SOURCES += main.cpp \
    runtime_settings.cpp \
    runners/main_runner.cpp \
    runners/abstract_runner.cpp

HEADERS += \
    runtime_settings.h \
    runners/main_runner.h \
    runners/abstract_runner.h

