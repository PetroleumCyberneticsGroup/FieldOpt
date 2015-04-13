#-------------------------------------------------
#
# Project created by QtCreator 2015-04-13T16:03:30
#
#-------------------------------------------------

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = fieldopt

DEFINES += GSSOPTIMIZER_LIBRARY

SOURCES += gssoptimizer.cpp

HEADERS += gssoptimizer.h\
        gssoptimizer_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
