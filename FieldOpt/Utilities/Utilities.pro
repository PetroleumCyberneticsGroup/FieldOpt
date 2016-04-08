include(../defaults.pri)

CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = utilities

HEADERS += \
    settings/settings_exceptions.h \
    settings/optimizer.h \
    settings/simulator.h \
    settings/model.h \
    settings/settings.h \
    file_handling/filehandling.h \
    file_handling/filehandling_exceptions.h \
    unix/execution.h

SOURCES += \
    settings/optimizer.cpp \
    settings/simulator.cpp \
    settings/model.cpp \
    settings/settings.cpp \
    file_handling/filehandling.cpp \
    unix/execution.cpp

DISTFILES += \
    README.md \
    settings/README.md
