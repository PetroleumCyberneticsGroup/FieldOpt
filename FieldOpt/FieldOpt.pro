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
    WellIndexCalculator/WellIndexCalculatorTests \
    GTest \
    AdgprsResultsReader

GTest.depends = ERTWrapper Utilities Model Optimization Simulation Runner AdgprsResultsReader
Model.depends = ERTWrapper Utilities WellIndexCalculator
Optimization.depends = Model Utilities Simulation
Simulation.depends = Model Utilities AdgprsResultsReader
Runner.depends = Optimization Model Utilities Simulation

OTHER_FILES += \
    defaults.pri \
    ../examples/MRST/compass/driver.dat \
    ../examples/MRST/compass/driver_kongull.dat \
    AdgprsSummaryConverter/*

# Copy ADPGRS summary converter python script to build dir
copy_scripts.commands = \
    $(MKDIR) -p $$OUT_PWD/AdgprsSummaryConverter ; \
    $(MKDIR) -p $$OUT_PWD/execution_scripts ; \
    $(COPY_FILE) $$PWD/Simulation/execution_scripts/*.sh $$OUT_PWD/execution_scripts ; \
    $(COPY_FILE) $$PWD/AdgprsSummaryConverter/*.py $$OUT_PWD/AdgprsSummaryConverter/
first.depends = $(first) copy_scripts
export(first.depends)
export(copy_scripts.commands)
QMAKE_EXTRA_TARGETS  += first copy_scripts
