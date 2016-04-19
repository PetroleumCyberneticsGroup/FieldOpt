include(../defaults.pri)


CONFIG   -= app_bundle
CONFIG += c++11

LIBS += -L$$OUT_PWD/../ERTWrapper -lertwrapper
LIBS += -L$$OUT_PWD/../Utilities -lutilities
LIBS += -L$$OUT_PWD/../WellIndexCalculator -lWellIndexCalculator

LIBS += -lrpoly_plus_plus

TEMPLATE = lib

TARGET = model

HEADERS += \
    model.h \
    wells/well.h \
    reservoir/reservoir.h \
    reservoir/grid/cell.h \
    reservoir/grid/grid.h \
    reservoir/grid/grid_exceptions.h \
    reservoir/grid/ijkcoordinate.h \
    reservoir/grid/xyzcoordinate.h \
    wells/wellbore/completions/completion.h \
    wells/wellbore/completions/perforation.h \
    wells/wellbore/trajectory.h \
    wells/wellbore/wellspline.h \
    wells/wellbore/completions/completion_exceptions.h \
    reservoir/grid/eclgrid.h \
    wells/control.h \
    wells/well_exceptions.h \
    wells/wellbore/wellblock.h \
    reservoir/reservoir_exceptions.h \
    properties/property.h \
    properties/binary_property.h \
    properties/property_exceptions.h \
    properties/variable_property_container.h \
    properties/variable_property_handler.h \
    properties/continous_property.h \
    properties/discrete_property.h

SOURCES += \
    model.cpp \
    wells/well.cpp \
    reservoir/reservoir.cpp \
    reservoir/grid/cell.cpp \
    reservoir/grid/grid.cpp \
    reservoir/grid/ijkcoordinate.cpp \
    reservoir/grid/xyzcoordinate.cpp \
    wells/wellbore/completions/completion.cpp \
    wells/wellbore/completions/perforation.cpp \
    wells/wellbore/trajectory.cpp \
    wells/wellbore/wellspline.cpp \
    reservoir/grid/eclgrid.cpp \
    wells/control.cpp \
    wells/wellbore/wellblock.cpp \
    properties/binary_property.cpp \
    properties/variable_property_container.cpp \
    properties/variable_property_handler.cpp \
    properties/continous_property.cpp \
    properties/discrete_property.cpp
