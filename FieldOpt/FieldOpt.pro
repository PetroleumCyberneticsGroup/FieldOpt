TEMPLATE = subdirs
CONFIG += c++11

SUBDIRS = \
    ERTWrapper \
    GTest \
    Model \
    Utilities \
    Optimization \
    Simulation \
    Runner \
    WellIndexCalculator \
    WellIndexCalculator/WellIndexCalculatorTests

GTest.depends = ERTWrapper Utilities Model Optimization Simulation Runner
Model.depends = ERTWrapper Utilities
Optimization.depends = Model Utilities Simulation
Simulation.depends = Model Utilities
Runner.depends = Optimization Model Utilities Simulation
WellIndexCalculator.depends = Model

OTHER_FILES += \
    defaults.pri \
    ../examples/MRST/compass/driver.dat \
    ../examples/MRST/compass/driver_kongull.dat

# Copy simulator execution scripts to build dir
copy_sim_exec_scripts.commands = $(COPY_DIR) $$PWD/execution_scripts $$OUT_PWD
first.depends = $(first) copy_sim_exec_scripts
export(first.depends)
export(copy_sim_exec_scripts.commands)
QMAKE_EXTRA_TARGETS += first copy_sim_exec_scripts
