#ifndef POSIXUTILITIES_H
#define POSIXUTILITIES_H

#include <iostream>
#include <stdio.h>
#include <QString>

class PosixUtilities
{
public:
    PosixUtilities();

    static bool executeCommand(QString cmd, QString workdir);
};

#endif // POSIXUTILITIES_H
