/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
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

#include "driverpart.h"
#include "Simulation/simulator_interfaces/simulator_exceptions.h"
#include <QRegExp>

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {

QString DriverPart::getSectionContent(QStringList *driver_file_content, QString keyword, QString next_keyword)
{
    int start_index = getLineIndex(driver_file_content, keyword);
    int end_index = getLineIndex(driver_file_content, next_keyword);
    if (start_index == -1) throw UnableToFindKeywordException(keyword);
    if (end_index == -1) throw UnableToFindKeywordException(next_keyword);

    return getLines(driver_file_content, start_index, end_index);
}

QString DriverPart::getSectionContent(QStringList *driver_file_content, QString keyword, QStringList possible_next_keywords)
{
    int start_index = getLineIndex(driver_file_content, keyword);
    if (start_index == -1) throw UnableToFindKeywordException(keyword);

    QList<int> possible_next_indices = QList<int>();
    foreach (QString possible_keyword, possible_next_keywords) {
        QString regex_string = "^" + possible_keyword + ".*";
        int possible_index = driver_file_content->indexOf(QRegExp(regex_string));
        if (possible_index != -1) possible_next_indices.append(possible_index);
    }
    if (possible_next_indices.size() == 0)
        throw UnableToFindKeywordException(possible_next_keywords.join(", "));

    // The end index we want is the lowest one.
    int end_index = possible_next_indices.first();
    foreach (int possible_index, possible_next_indices)
        if (possible_index < end_index) end_index = possible_index;

    return getLines(driver_file_content, start_index, end_index);
}

QString DriverPart::getLines(QStringList *driver_file_content, int start_index, int end_index)
{
    QStringList content = QStringList(driver_file_content->mid(start_index, end_index - start_index));
    return content.join("\n");
}

int DriverPart::getLineIndex(QStringList *driver_file_content, QString keyword)
{
    QString keyword_regex_string = "^" + keyword + ".*";
    int index = driver_file_content->indexOf(QRegExp(keyword_regex_string));
    return index;
}




}
}
}
}
