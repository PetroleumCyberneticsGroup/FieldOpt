include(../defaults.pri)

QT += testlib
QT -= gui
TARGET = Test
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
TEMPLATE = app
LIBS += -L../src -lfieldopt

HEADERS += \
    autotest.h \
    testvariable.h \
    testconstraint.h \
    teststream.h \
    testbeggsbrillcalculator.h

SOURCES += \
    main.cpp \
    testvariable.cpp \
    testconstraint.cpp \
    teststream.cpp \
    testbeggsbrillcalculator.cpp
