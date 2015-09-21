/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
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
 */

#include "exceptionhandler.h"
#include <QTextStream>

bool ExceptionHandler::verbose = false;

ExceptionHandler::ExceptionHandler()
{//
    warning_header = "\n************************************************************\n"
                     "********************       WARNING      ********************\n"
                     "************************************************************\n";

    warning_footer = "********************    END  WARNING    ********************\n\n";

    error_header = "\n************************************************************\n"
                   "********************        ERROR       ********************\n"
                   "************************************************************\n";

    error_footer = "********************      END ERROR     ********************\n\n";

    progress_header = "\n********************       PROGRESS     ********************\n";
}

void ExceptionHandler::printWarning(QString message, ExceptionType type)
{
    QString l1 = "Exception type:\t" + getTypeString(type) + "\n";
    QString l2 = "Message:\t" + message + "\n";
    if (verbose == true)
        QTextStream(stdout) << QString("%1 \n %2 \n %3 \n%4").arg(warning_header).arg(l1).arg(l2).arg(warning_footer);
}

void ExceptionHandler::printError(QString message, ExceptionType type)
{
    QString l1 = "Exception type: " + getTypeString(type) + "\n";
    QString l2 = "Message: " + message + "\n";
        QTextStream(stderr) << QString("%1 \n %2 \n %3 \n %4").arg(error_header).arg(l1).arg(l2).arg(error_footer);
}

void ExceptionHandler::printProgress(QString message)
{
    if (verbose == true)
        QTextStream(stdout) << QString("%1%2\n").arg(progress_header).arg(message);
}
