include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = fieldopt

HEADERS += \
    constraints/integerboundaryconstraint.h \
    constraints/doubleboundaryconstraint.h \
    variables/variable.h \
    variables/intvariable.h \
    variables/realvariable.h \
    variables/binaryvariable.h \
    exceptionhandler/exceptionhandler.h \
    exceptionhandler/variablehandler.h \
    constraints/constraint.h \
    model/stream.h \
    printers/printer.h \
    printers/streamprinter.h \
    model/components/component.h \
    pressuredropcalculators/pressuredropcalculator.h \
    pressuredropcalculators/beggsbrillcalculator.h \
    model/components/pipe.h \
    utilities/stringutilities.h \
    exceptionhandler/componenthandler.h \
    model/wellcontrol.h \
    model/wellconnection.h \
    model/wellconnectionvariable.h \
    model/wellpath.h \
    model/components/productionwell.h \
    model/components/well.h \
    model/cost.h \
    model/pipeconnection.h \
    fileio/readeres/pipereader.h \
    fileio/readeres/reader.h \
    exceptionhandler/readerhandler.h \
    model/components/midpipe.h \
    model/components/injectionwell.h \
    model/components/endpipe.h \
    constraints/userconstraint.h \
    exceptionhandler/constrainthandler.h \
    model/components/separator.h \
    model/reservoir.h \
    model/model.h \
    model/objectives/objective.h \
    model/objectives/cumgasobjective.h \
    model/objectives/cumoilobjective.h \
    exceptionhandler/objectivehandler.h \
    model/objectives/npvobjective.h \
    constraints/capacity.h \
    exceptionhandler/modelhandler.h \
    model/components/pressurebooster.h \
    settings/runtimesettings.h \
    settings/optimizersettings.h \
    settings/simulatorsettings.h \
    simulators/simulator.h \
    exceptionhandler/optimizerhandler.h \
    optimizers/optimizer.h \
    optimizers/case.h \
    model/coupledmodel.h \
    simulators/mrstbatchsimulator.h \
    optimizers/derivative.h \
    printers/caseprinter.h \
    optimizers/adjoint.h \
    optimizers/adjointcollection.h \
    fileio/readeres/driverreader.h \
    model/adjointscoupledmodel.h \
    model/inputratevariable.h \
    constraints/materialbalanceconstraint.h \
    model/decoupledmodel.h \
    exceptionhandler/simulatorhandler.h \
    utilities/posixutilities.h \
    optimizers/gssalgorithm.h \
    optimizers/compasssearchoptimizer.h \
    optimizers/bookkeeper.h \
    exceptionhandler/casehandler.h \
    model/grid/grid.h \
    model/grid/cell.h \
    model/grid/ijkcoordinate.h \
    model/grid/xyzcoordinate.h

SOURCES += \
    constraints/integerboundaryconstraint.cpp \
    constraints/doubleboundaryconstraint.cpp \
    variables/intvariable.cpp \
    variables/variable.cpp \
    variables/realvariable.cpp \
    variables/binaryvariable.cpp \
    exceptionhandler/exceptionhandler.cpp \
    exceptionhandler/variablehandler.cpp \
    constraints/constraint.cpp \
    model/stream.cpp \
    printers/printer.cpp \
    printers/streamprinter.cpp \
    model/components/component.cpp \
    pressuredropcalculators/beggsbrillcalculator.cpp \
    model/components/pipe.cpp \
    utilities/stringutilities.cpp \
    exceptionhandler/componenthandler.cpp \
    model/wellcontrol.cpp \
    model/wellconnection.cpp \
    model/wellconnectionvariable.cpp \
    model/wellpath.cpp \
    model/components/productionwell.cpp \
    model/components/well.cpp \
    model/cost.cpp \
    model/pipeconnection.cpp \
    fileio/readeres/pipereader.cpp \
    fileio/readeres/reader.cpp \
    exceptionhandler/readerhandler.cpp \
    model/components/midpipe.cpp \
    model/components/injectionwell.cpp \
    model/components/endpipe.cpp \
    constraints/userconstraint.cpp \
    exceptionhandler/constrainthandler.cpp \
    model/components/separator.cpp \
    model/reservoir.cpp \
    model/model.cpp \
    model/objectives/cumgasobjective.cpp \
    model/objectives/cumoilobjective.cpp \
    exceptionhandler/objectivehandler.cpp \
    model/objectives/objective.cpp \
    model/objectives/npvobjective.cpp \
    constraints/capacity.cpp \
    exceptionhandler/modelhandler.cpp \
    model/components/pressurebooster.cpp \
    settings/runtimesettings.cpp \
    settings/optimizersettings.cpp \
    settings/simulatorsettings.cpp \
    simulators/simulator.cpp \
    exceptionhandler/optimizerhandler.cpp \
    optimizers/optimizer.cpp \
    optimizers/case.cpp \
    model/coupledmodel.cpp \
    simulators/mrstbatchsimulator.cpp \
    optimizers/derivative.cpp \
    printers/caseprinter.cpp \
    optimizers/adjoint.cpp \
    optimizers/adjointcollection.cpp \
    model/adjointscoupledmodel.cpp \
    fileio/readeres/driverreader.cpp \
    model/inputratevariable.cpp \
    constraints/materialbalanceconstraint.cpp \
    model/decoupledmodel.cpp \
    exceptionhandler/simulatorhandler.cpp \
    utilities/posixutilities.cpp \
    optimizers/compasssearchoptimizer.cpp \
    optimizers/bookkeeper.cpp \
    exceptionhandler/casehandler.cpp \
    model/grid/grid.cpp \
    model/grid/cell.cpp \
    model/grid/ijkcoordinate.cpp \
    model/grid/xyzcoordinate.cpp
