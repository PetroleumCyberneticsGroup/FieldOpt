include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = lib

TARGET = fieldopt

HEADERS += \
    variables/variable.h \
    variables/intvariable.h \
    variables/realvariable.h \
    variables/binaryvariable.h

SOURCES += \
    variables/intvariable.cpp \
    variables/variable.cpp \
    variables/realvariable.cpp \
    variables/binaryvariable.cpp
