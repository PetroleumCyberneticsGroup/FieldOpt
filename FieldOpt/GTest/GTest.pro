include(../defaults.pri)
QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
LIBS += -L../Model -lmodel
LIBS += -L../ERTWrapper -lertwrapper
LIBS += -lpthread -lgtest -pthread
SOURCES += \
    gtest_main.cpp \
    ERTWrapper/test_eclgridreader.cpp \
    Model/reservoir/grid/test_cell.cpp \
    Model/reservoir/grid/test_grid.cpp \
    Model/reservoir/grid/test_ijkcoordinate.cpp \
    Model/reservoir/grid/test_xyzcoordinate.cpp \
    Model/variables/test_variables.cpp
