include(../defaults.pri)
QT       += core
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11
LIBS += -L../src -lfieldopt
LIBS += -lecl -lutil
TEMPLATE = lib
TARGET = ertwrapper

HEADERS += \
    eclgridreader.h

SOURCES += \
    eclgridreader.cpp
