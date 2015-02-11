include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = fieldopt

HEADERS += \
    variables/variable.h \
    variables/intvariable.h \
    variables/realvariable.h \
    variables/binaryvariable.h \
    exceptionhandler/exceptionhandler.h \
    exceptionhandler/variablehandler.h \
    constraints/constraint.h \
    model/stream.h \
    printers/printer.h \
    printers/streamprinter.h \
    model/components/component.h

SOURCES += \
    variables/intvariable.cpp \
    variables/variable.cpp \
    variables/realvariable.cpp \
    variables/binaryvariable.cpp \
    exceptionhandler/exceptionhandler.cpp \
    exceptionhandler/variablehandler.cpp \
    constraints/constraint.cpp \
    model/stream.cpp \
    printers/printer.cpp \
    printers/streamprinter.cpp \
    model/components/component.cpp
