include(../defaults.pri)
QT       += core

QT       -= gui

TARGET = Console
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

# MPI Settings
QMAKE_CXX = mpicxx
QMAKE_CXX_RELEASE = $$QMAKE_CXX
QMAKE_CXX_DEBUG = $$QMAKE_CXX
QMAKE_LINK = $$QMAKE_CXX
QMAKE_CC = mpicc

QMAKE_CFLAGS += $$system(mpicc --showme:compile)
QMAKE_LFLAGS += $$system(mpicxx --showme:link)
QMAKE_CXXFLAGS += $$system(mpicxx --showme:compile) -DMPICH_IGNORE_CXX_SEEK
QMAKE_CXXFLAGS_RELEASE += $$system(mpicxx --showme:compile) -DMPICH_IGNORE_CXX_SEEK

LIBS += -lboost_mpi -lboost_serialization
LIBS += -L../src -lfieldopt


TEMPLATE = app


SOURCES += main.cpp \
    transferobjects/perturbation.cpp \
    transferobjects/result.cpp \
    simulationlauncher.cpp \
    masterrunner.cpp \
    parallelprinter.cpp \
    broker.cpp \
    resultslogger.cpp \
    summaryprinter.cpp \
    batchlogger.cpp

HEADERS += \
    transferobjects/perturbation.h \
    transferobjects/result.h \
    simulationlauncher.h \
    masterrunner.h \
    parallelprinter.h \
    broker.h \
    resultslogger.h \
    summaryprinter.h \
    batchlogger.h
