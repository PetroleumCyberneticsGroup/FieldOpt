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

#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H

#include <QString>
#include <QStringList>

/*!
 * \brief A class containing static utilities for checking and processing strings.
 */
class StringUtilities
{
public:
    /*!
     * \brief Process a line from the driver file.
     *
     * 1. Removes everything after the first "!" (comments).
     *
     * 2. Trim leading and trailing whitespace.
     *
     * 3. Split the string by whitespace.
     *
     * 4. Add a whitespace element at the end of the string list if the length is 1. \todo Why is this done?
     * \param line The line to be processed
     * \return A QStringList representing the line.
     */
    static QStringList processLine(const QString &line);

    /*!
     * \brief Check if the line is empty (i.e. contains only whitespace)
     * \param list QStringList representing the line, created by the processLine function.
     * \return Returns true if the line contains only whitespace; otherwise false.
     */
    static bool isEmpty(const QStringList &list);
};

#endif // STRINGUTILITIES_H
