CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = utilities

HEADERS += \
    settings/settings.h \
    settings/optimizer.h \
    settings/simulator.h \
    settings/model.h

SOURCES += \
    settings/settings.cpp \
    settings/optimizer.cpp \
    settings/simulator.cpp \
    settings/model.cpp
