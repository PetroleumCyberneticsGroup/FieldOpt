CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = utilities

HEADERS += \
    settings/settings_exceptions.h \
    settings/optimizer.h \
    settings/simulator.h \
    settings/model.h \
    settings/settings.h

SOURCES += \
    settings/optimizer.cpp \
    settings/simulator.cpp \
    settings/model.cpp \
    settings/settings.cpp
