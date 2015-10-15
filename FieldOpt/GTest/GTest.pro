include(../defaults.pri)
QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
LIBS += -L../Model -lmodel
LIBS += -L../Optimization -loptimization
LIBS += -L../ERTWrapper -lertwrapper
LIBS += -L../Utilities -lutilities
LIBS += -lpthread -lgtest -pthread
SOURCES += \
    gtest_main.cpp \
    ERTWrapper/test_eclgridreader.cpp \
    Model/reservoir/grid/test_cell.cpp \
    Model/reservoir/grid/test_grid.cpp \
    Model/reservoir/grid/test_ijkcoordinate.cpp \
    Model/reservoir/grid/test_xyzcoordinate.cpp \
    Model/variables/test_variables.cpp \
    Model/variables/test_variablecontainer.cpp \
    Utilities/settings/test_settings.cpp \
    Utilities/settings/test_simulator.cpp \
    Utilities/settings/test_model.cpp \
    Utilities/settings/test_optimizer.cpp \
    ERTWrapper/test_eclsummaryreader.cpp \
    Model/results/test_eclresults.cpp \
    Model/objective/test_weightedsum.cpp

OTHER_FILES += \
    Utilities/driver/driver.json
