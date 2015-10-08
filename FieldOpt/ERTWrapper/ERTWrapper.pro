include(../defaults.pri)
QT       += core
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11
LIBS += -lecl -lutil
TEMPLATE = lib
TARGET = ertwrapper

HEADERS += \
    eclgridreader.h \
    ertwrapper_exceptions.h \
    eclsummaryreader.h

SOURCES += \
    eclgridreader.cpp \
    eclsummaryreader.cpp
