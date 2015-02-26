include(../defaults.pri)

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = lib

TARGET = fieldopt

HEADERS += \
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
    exceptionhandler/readerhandler.h

SOURCES += \
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
    exceptionhandler/readerhandler.cpp
