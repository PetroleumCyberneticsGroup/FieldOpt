include(../defaults.pri)

CONFIG   -= app_bundle
CONFIG += c++11
TEMPLATE = lib

LIBS += -lhdf5_cpp

TARGET = adgprsresultsreader

SOURCES += \
    json_summary_reader.cpp \
    adgprs_results_reader.cpp

HEADERS += \
    json_summary_reader.h \
    adgprs_results_reader.h
