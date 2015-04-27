TEMPLATE = subdirs
CONFIG+=ordered
CONFIG += c++11
SUBDIRS = \
    src \
    Console  #\
    #tests

Console.depends = src
tests.depends = src

OTHER_FILES += \
    defaults.pri \
    ../examples/MRST/compass/driver.dat
