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

#include "stringutilities.h"
#include <QRegExp>

QStringList StringUtilities::processLine(const QString &line) const
{
    QString temp = line.split("!").first();  // Remove everything after "!" (comments).
    temp = temp.trimmed();  // Remove leading and trailing whitespace.
    QStringList list = temp.split(QRegExp("\\s+"));  // Split line at whitespace.
    while (list.size() < 2)
    {
        list.push_back(" ");
    }
    return list;
}

bool StringUtilities::isEmpty(const QStringList &list) const
{
    bool empty = true;
    for (int i = 0; i < list.size(); i++)
    {
        QString temp = list.at(i).remove(QRegExp("\\s+"));  // Remove all whitespace from element i
        if (temp.size() != 0) {  // Return false if an element in the list contains anything after removing all whitespace.
            empty = false;
            break;
        }
    }
    return empty;
}
