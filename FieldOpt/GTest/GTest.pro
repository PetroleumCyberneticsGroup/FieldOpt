include(../defaults.pri)
QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
LIBS += -L../Model -lmodel
LIBS += -L../Optimization -loptimization
LIBS += -L../Simulation -lsimulation
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
    ERTWrapper/test_eclsummaryreader.cpp \
    Model/results/test_eclresults.cpp \
    Model/objective/test_weightedsum.cpp \
    Model/variables/test_variablehandler.cpp \
    Model/wells/test_control.cpp \
    Model/wells/test_perforation.cpp \
    Utilities/file_handling/test_filehandling.cpp \
    Model/wells/test_wellblock.cpp \
    Model/wells/test_trajectory.cpp \
    Model/reservoir/test_reservoir.cpp \
    Model/test_model.cpp \
    Utilities/settings/test_settings_model.cpp \
    Utilities/settings/test_settings_optimizer.cpp \
    Utilities/settings/test_settings_simulator.cpp \
    Model/wells/test_well.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_welspecs.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_compdat.cpp

OTHER_FILES += \
    Utilities/driver/driver.json

HEADERS += \
    Model/test_fixture_model_base.h
