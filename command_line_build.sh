#!/bin/sh
rm -rf build
mkdir build
cd build

qmake "../FieldOpt/FieldOpt.pro" -r -spec linux-g++ CONFIG+=debug "QMAKE_CXXFLAGS+=-std=c++11"
make

