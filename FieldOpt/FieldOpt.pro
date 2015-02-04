TEMPLATE = subdirs
CONFIG+=ordered
SUBDIRS = \
    src \
    Console \
    tests

Console.depends = src
tests.depends = src

OTHER_FILES += \
    defaults.pri
