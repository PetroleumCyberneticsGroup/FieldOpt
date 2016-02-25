TEMPLATE = subdirs
CONFIG += c++11

# GTest has been omitted from the release
SUBDIRS = \
    ERTWrapper \
    Model \
    Utilities \
    Optimization \
    Simulation \
    Runner \
    WellIndexCalculator \
    GTest \
    AdgprsResultsReader

GTest.depends = ERTWrapper Utilities Model Optimization Simulation Runner AdgprsResultsReader
Model.depends = ERTWrapper Utilities
Optimization.depends = Model Utilities Simulation
Simulation.depends = Model Utilities AdgprsResultsReader
Runner.depends = Optimization Model Utilities Simulation
WellIndexCalculator.depends = Model

OTHER_FILES += \
    defaults.pri \
    ../examples/MRST/compass/driver.dat \
    ../examples/MRST/compass/driver_kongull.dat \
    AdgprsSummaryConverter/*

# Copy simulator execution scripts to build dir
copy_sim_exec_scripts.commands = $(COPY_DIR) $$PWD/execution_scripts $$OUT_PWD
first.depends = $(first) copy_sim_exec_scripts
export(first.depends)
export(copy_sim_exec_scripts.commands)
QMAKE_EXTRA_TARGETS += first copy_sim_exec_scripts

# Copy ADPGRS summary converter python script to build dir
copy_python_adgprs_summary_converter.commands = $(MKDIR) -p $$OUT_PWD/AdgprsSummaryConverter ; $(COPY_FILE) $$PWD/AdgprsSummaryConverter/*.py $$OUT_PWD/AdgprsSummaryConverter/
first.depends = $(first) copy_python_adgprs_summary_converter
export(first.depends)
export(copy_python_adgprs_summary_converter.commands)
QMAKE_EXTRA_TARGETS  += first copy_python_adgprs_summary_converter
