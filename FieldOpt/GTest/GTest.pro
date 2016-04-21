include(../defaults.pri)
QT       += core
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
LIBS += -L$$OUT_PWD/../Model -lmodel
LIBS += -L$$OUT_PWD/../Optimization -loptimization
LIBS += -L$$OUT_PWD/../Simulation -lsimulation
LIBS += -L$$OUT_PWD/../ERTWrapper -lertwrapper
LIBS += -L$$OUT_PWD/../Utilities -lutilities
LIBS += -L$$OUT_PWD/../AdgprsResultsReader -ladgprsresultsreader
LIBS += -L$$OUT_PWD/../WellIndexCalculator -lWellIndexCalculator
LIBS += -L$$OUT_PWD/../Runner -lFieldOpt

LIBS += -lpthread -lgtest -pthread -lrpoly_plus_plus
SOURCES += \
    gtest_main.cpp \
    ERTWrapper/test_eclgridreader.cpp \
    Model/reservoir/grid/test_cell.cpp \
    Model/reservoir/grid/test_grid.cpp \
    Model/reservoir/grid/test_ijkcoordinate.cpp \
    Model/reservoir/grid/test_xyzcoordinate.cpp \
    Utilities/settings/test_settings.cpp \
    ERTWrapper/test_eclsummaryreader.cpp \
    Model/results/test_eclresults.cpp \
    Optimization/objective/test_weightedsum.cpp \
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
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_compdat.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_wellcontrols.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_grid_section.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_runspec_section.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_props_section.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_solution_section.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_summary_section.cpp \
    Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/test_schedule_section.cpp \
    Simulation/simulator_interfaces/test_eclsimulator.cpp \
    Utilities/unix/test_execution.cpp \
    Model/variables/test_properties.cpp \
    Model/variables/test_variable_property_container.cpp \
    Model/variables/test_variable_property_handler.cpp \
    Optimization/test_case.cpp \
    Optimization/test_case_handler.cpp \
    Optimization/constraints/test_box_constraint.cpp \
    Optimization/constraints/test_constraint_handler.cpp \
    Optimization/optimizers/test_compass_search.cpp \
    Runner/test_bookkeeper.cpp \
    Simulation/results/test_adgprsresults.cpp \
    AdgprsResultsReader/test_json_summary_reader.cpp \
    AdgprsResultsReader/test_adgprs_results_reader.cpp \
    Simulation/simulator_interfaces/test_adgprssimulator.cpp \
    Simulation/simulator_interfaces/driver_file_writers/adgprs_driver_file_writer.cpp

OTHER_FILES += \
    Utilities/driver/driver.json

HEADERS += \
    Model/test_fixture_model_base.h \
    Optimization/test_fixture_case.h \
    Optimization/test_fixture_optimizer.h \
    Simulation/results/test_fixture_adgprs.h
