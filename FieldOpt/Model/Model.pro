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
    variables/binaryvariable.h \
    variables/variablecontainer.h \
    wells/completions/completion.h \
    wells/completions/perforation.h \
    wells/completions/inflowcontroldevice.h \
    wells/trajectory/trajectory.h \
    wells/trajectory/wellspline.h \
    wells/trajectory/wellblocks.h \
    wells/wellgroup.h \
    wells/completions/completioncollection.h \
    wells/completions/completion_exceptions.h \
    objective/weightedsum.h \
    results/results.h \
    results/eclresults.h \
    results/results_exceptions.h

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
    variables/binaryvariable.cpp \
    variables/variablecontainer.cpp \
    wells/completions/completion.cpp \
    wells/completions/perforation.cpp \
    wells/completions/inflowcontroldevice.cpp \
    wells/trajectory/trajectory.cpp \
    wells/trajectory/wellspline.cpp \
    wells/trajectory/wellblocks.cpp \
    wells/wellgroup.cpp \
    wells/completions/completioncollection.cpp \
    objective/weightedsum.cpp \
    results/eclresults.cpp
