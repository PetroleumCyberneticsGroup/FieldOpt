TEMPLATE = subdirs
CONFIG += c++11

SUBDIRS = \
    ERTWrapper \
    GTest \
    Model \
    Utilities \
    Optimization \
    Simulation \
    Runner

GTest.depends = ERTWrapper Utilities Model Optimization Simulation Runner
Model.depends = ERTWrapper Utilities
Optimization.depends = Model Utilities Simulation
Simulation.depends = Model Utilities
Runner.depends = Optimization Model Utilities Simulation

OTHER_FILES += \
    defaults.pri \
    ../examples/MRST/compass/driver.dat \
    ../examples/MRST/compass/driver_kongull.dat
