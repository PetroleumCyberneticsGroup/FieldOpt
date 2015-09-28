CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = utilities

HEADERS += \
    driver/reader.h \
    settings/settings.h \
    settings/optimizer.h \
    settings/simulator.h \
    settings/model.h

SOURCES += \
    driver/reader.cpp \
    settings/settings.cpp \
    settings/optimizer.cpp \
    settings/simulator.cpp \
    settings/model.cpp
