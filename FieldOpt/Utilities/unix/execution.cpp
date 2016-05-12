/******************************************************************************
 *
 *
 *
 * Created: 23.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "execution.h"
#include "Utilities/file_handling/filehandling.h"
#include "Utilities/file_handling/filehandling_exceptions.h"
#include <iostream>
#include <QString>
#include <QStringList>
#include <stdlib.h>
#include <stdio.h>
#include <QFileInfo>

namespace Utilities { namespace Unix {

QString Exec(QString directory, QString command)
{
    return Exec(directory, QStringList(command));
}

QString Exec(QString directory, QStringList commands)
{
    return Exec(directory, commands, false);
}

QString Exec(QString directory, QString command, bool verbose)
{
    return Exec(directory, QStringList(command), verbose);
}

QString Exec(QString directory, QStringList commands, bool verbose)
{
    if (!Utilities::FileHandling::DirectoryExists(directory))
        throw Utilities::FileHandling::DirectoryNotFoundException("Directory for command execution not found", directory);

    QString cmd = "cd " + directory + "; " + commands.join("; ");
    FILE* pipe = popen(cmd.toStdString().c_str(), "r");
    if (!pipe) throw std::runtime_error("Unable to open pipe");
    char buffer[256];
    QString result = "";
    while(!feof(pipe)) {
        if (fgets(buffer, 256, pipe) != NULL) {
            result.append(QString(buffer));
            if (verbose)
                std::cout << buffer << std::endl;
        }

    }
    pclose(pipe);
    return result;
}

void ExecShellScript(QString script_path, QStringList args)
{
    if (!Utilities::FileHandling::FileExists(script_path))
        throw ::Utilities::FileHandling::FileNotFoundException(script_path);
    QString command = script_path + " " + args.join(" ");
    system(command.toLatin1().constData());
}

}}
