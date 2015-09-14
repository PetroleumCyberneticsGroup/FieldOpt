include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = fieldopt

HEADERS += \
    eclgridreader.h

SOURCES += \
    eclgridreader.cpp
