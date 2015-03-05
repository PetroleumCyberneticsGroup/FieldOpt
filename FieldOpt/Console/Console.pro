#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T11:06:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Console
CONFIG   += console
CONFIG   -= app_bundle

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

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_mpi -lboost_serialization


TEMPLATE = app


SOURCES += main.cpp
