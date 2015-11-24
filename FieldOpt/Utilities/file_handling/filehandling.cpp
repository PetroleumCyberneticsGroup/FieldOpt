/******************************************************************************
 *
 *
 *
 * Created: 28.10.2015 2015 by einar
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

#include "filehandling.h"
#include "filehandling_exceptions.h"
#include <QTextStream>
#include <QDir>
#include <iostream>

bool Utilities::FileHandling::FileExists(QString file_path)
{
    QFileInfo file(file_path);
    QFileInfo file_relative(file.absoluteFilePath());
    if (file.exists() && file.isFile())
        return true;
    else if (file_relative.exists() && file_relative.isFile())
        return true;
    else return false;
}

bool Utilities::FileHandling::DirectoryExists(QString directory_path)
{
    QFileInfo folder(directory_path);
    if (folder.exists() && folder.isDir()) return true;
    else return false;
}

bool Utilities::FileHandling::ParentDirectoryExists(QString file_path)
{
    QFileInfo file(file_path);
    QDir parent_directory = file.dir();
    if (parent_directory.exists())
        return true;
    else
        return false;
}

QStringList *Utilities::FileHandling::ReadFileToStringList(QString file_path)
{
    QStringList *string_list = new QStringList();
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw FileNotFoundException(file_path);

    QTextStream text_stream(&file);
    while (true) {
        QString line = text_stream.readLine();
        if (line.isNull())
            break;
        else
            string_list->append(line);
    }
    file.close();
    return string_list;
}

void Utilities::FileHandling::WriteStringToFile(QString string, QString file_path)
{
    if (!ParentDirectoryExists(file_path))
        throw DirectoryNotFoundException("Specified file's parent directory does not exist", file_path);

    QFile file(file_path);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream out(&file);
    out << string.toUtf8() << endl;
    file.close();
}

QString Utilities::FileHandling::GetBuildDirectoryPath()
{
    QDir path = QDir::currentPath(); // Get current directory
    while (path.cdUp()) { // cd up untill we hit a directory that starts with 'build-'
        QString current = path.dirName();
        if (QString::compare(current.split("-").first(), "build") == 0)
            break;
        else if (QString::compare(path.dirName(), "home") == 0) // If we're in the home directory, we've come too far.
            throw DirectoryNotFoundException("Unable to find the build directory.", "");
    }
    return path.absolutePath();
}

QString Utilities::FileHandling::GetProjectDirectoryPath()
{
    QDir path(GetBuildDirectoryPath());
    path.cdUp();
    return path.absolutePath();
}
