#!/bin/sh
qmake FieldOpt/FieldOpt.pro -r -spec linux-g++ CONFIG+=debug "QMAKE_CXXFLAGS+=-std=c++11"
qmake && make

