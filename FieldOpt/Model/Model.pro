include(../defaults.pri)


CONFIG   -= app_bundle
CONFIG += c++11

LIBS += -L../ERTWrapper -lertwrapper
LIBS += -L../Utilities -lutilities

TEMPLATE = lib

TARGET = model

HEADERS += \
    model.h \
    wells/well.h \
    reservoir/reservoir.h \
    variables/variable.h \
    reservoir/grid/cell.h \
    reservoir/grid/grid.h \
    reservoir/grid/grid_exceptions.h \
    reservoir/grid/ijkcoordinate.h \
    reservoir/grid/xyzcoordinate.h \
    variables/integervariable.h \
    variables/variable_exceptions.h \
    variables/realvariable.h \
    variables/binaryvariable.h \
    variables/variablecontainer.h \
    wells/wellbore/completions/completion.h \
    wells/wellbore/completions/perforation.h \
    wells/wellbore/trajectory.h \
    wells/wellbore/wellspline.h \
    wells/wellbore/completions/completion_exceptions.h \
    results/results.h \
    results/eclresults.h \
    results/results_exceptions.h \
    reservoir/grid/eclgrid.h \
    wells/control.h \
    variables/variablehandler.h \
    wells/well_exceptions.h \
    wells/wellbore/wellblock.h \
    reservoir/reservoir_exceptions.h

SOURCES += \
    model.cpp \
    wells/well.cpp \
    reservoir/reservoir.cpp \
    reservoir/grid/cell.cpp \
    reservoir/grid/grid.cpp \
    reservoir/grid/ijkcoordinate.cpp \
    reservoir/grid/xyzcoordinate.cpp \
    variables/integervariable.cpp \
    variables/realvariable.cpp \
    variables/binaryvariable.cpp \
    variables/variablecontainer.cpp \
    wells/wellbore/completions/completion.cpp \
    wells/wellbore/completions/perforation.cpp \
    wells/wellbore/trajectory.cpp \
    wells/wellbore/wellspline.cpp \
    results/eclresults.cpp \
    reservoir/grid/eclgrid.cpp \
    wells/control.cpp \
    variables/variablehandler.cpp \
    wells/wellbore/wellblock.cpp
