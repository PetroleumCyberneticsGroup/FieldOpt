include(../defaults.pri)
QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
LIBS += -L../src -lfieldopt
LIBS += -L../ERTWrapper -lertwrapper
LIBS += -lpthread -lgtest -pthread
SOURCES += \
    gtest_main.cpp \
    ERTWrapper/test_eclgridreader.cpp \
    Library/model/grid/test_grid.cpp
