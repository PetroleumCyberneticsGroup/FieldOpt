include(../defaults.pri)

CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = model

HEADERS += \
    model.h \
    wells/well.h \
    reservoir/reservoir.h \
    schedule/schedule.h \
    objective/objective.h \
    variables/variable.h

SOURCES += \
    model.cpp \
    wells/well.cpp \
    reservoir/reservoir.cpp \
    schedule/schedule.cpp \
    objective/objective.cpp \
    variables/variable.cpp
