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

#ifndef EXECUTION_H
#define EXECUTION_H

#include <QString>
#include <QStringList>

namespace Utilities { namespace Unix {

/*!
 * \brief Exec Execute a command using a POSIX pipe.
 * \param directory The directory in which to execute the command.
 * \param command The command to execute.
 * \return The output from the command.
 */
QString Exec(QString directory, QString command);

/*!
 * \brief Exec Execute a list of commands using a POSIX pipe.
 * \param directory The directory in which to execute the command.
 * \param commands The commands to be executed.
 * \return The output from the command.
 */
QString Exec(QString directory, QStringList commands);

/*!
 * \brief Exec Execute a command using a POSIX pipe.
 * \param directory The directory in which to execute the command.
 * \param command The command to execute.
 * \param verbose Whether or not the output of the command should be displayed in the terminal.
 * \return The output from the command.
 */
QString Exec(QString directory, QString command, bool verbose);

/*!
 * \brief Exec Execute a list of commands using a POSIX pipe.
 * \param directory The directory in which to execute the command.
 * \param commands The commands to be executed.
 * \param verbose Whether or not the output of the command should be displayed in the terminal.
 * \return The output from the command.
 */
QString Exec(QString directory, QStringList commands, bool verbose);

/*!
 * \brief ExecShellScript Executes a shell script with the given set of parameters.
 * \param script_path Absolute path to the shell script.
 * \param args Arguments to be passed to the script.
 */
void ExecShellScript(QString script_path, QStringList args);

}}
#endif // EXECUTION_H

