include(../defaults.pri)

QT += testlib
QT -= gui
TARGET = Test
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
LIBS += -L../src -lfieldopt
