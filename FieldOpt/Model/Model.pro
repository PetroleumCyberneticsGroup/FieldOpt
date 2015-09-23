include(../defaults.pri)


CONFIG   -= app_bundle
CONFIG += c++11

LIBS += -L../ERTWrapper -lertwrapper

TEMPLATE = lib

TARGET = model

HEADERS += \
    model.h \
    wells/well.h \
    reservoir/reservoir.h \
    schedule/schedule.h \
    objective/objective.h \
    variables/variable.h \
    reservoir/grid/cell.h \
    reservoir/grid/grid.h \
    reservoir/grid/grid_exceptions.h \
    reservoir/grid/ijkcoordinate.h \
    reservoir/grid/xyzcoordinate.h \
    variables/integervariable.h \
    variables/variable_exceptions.h \
    variables/realvariable.h \
    variables/binaryvariable.h

SOURCES += \
    model.cpp \
    wells/well.cpp \
    reservoir/reservoir.cpp \
    schedule/schedule.cpp \
    objective/objective.cpp \
    reservoir/grid/cell.cpp \
    reservoir/grid/grid.cpp \
    reservoir/grid/ijkcoordinate.cpp \
    reservoir/grid/xyzcoordinate.cpp \
    variables/integervariable.cpp \
    variables/realvariable.cpp \
    variables/binaryvariable.cpp
