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

#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>

namespace Utilities {
namespace FileHandling {

/*!
 * \brief FileExists Checks whether or not a file exists at the specified path.
 * \param file_path Path to a file that may or may not exist.
 * \return True if a file exists at the specified path, otherwise false.
 */
bool FileExists(QString file_path);

/*!
 * \brief DirectoryExists Checks whether or not a folder exists at the specified path.
 * \param folder_path Path to a folder that may or may not exist.
 * \return True if a folder exists at the specified path, otherwise false.
 */
bool DirectoryExists(QString folder_path);

/*!
 * \brief DirectoryIsEmpty Check whether or not a directory is empty.
 * \param folder_path Path a folder to check.
 * \return True if the directory is empty, otherwise false.
 */
bool DirectoryIsEmpty(QString folder_path);

/*!
 * \brief ParentDirectoryExists Checks whether a specified file's parent directory exists.
 * \param file_path Path a file (the file itself does not have to exist).
 * \return True if the parent directory exists, otherwise false.
 */
bool ParentDirectoryExists(QString file_path);

/*!
 * \brief ReadFileToStringList Reads the contents of a file and stores it as
 * a string list where each element is a line in the file.
 * \param file_path The file to create a list from.
 * \return List where each element is a line in the file.
 */
QStringList *ReadFileToStringList(QString file_path);

/*!
 * \brief WriteStringToFile Write a string to a file. Removes existing file contents.
 *
 * If the string does not end with a newline, it will be added.
 * \param string The string to be written.
 * \param file_path Path to the file to write the string into.
 */
void WriteStringToFile(QString string, QString file_path);

/*!
 * \brief WriteLineToFile Append a string to a file.
 *
 * If the string does not end with a newline, it will be added.
 * \param string The string/line to be written.
 * \param file_path The file to write the string/line to.
 */
void WriteLineToFile(QString string, QString file_path);

/*!
 * \brief DeleteFile Deletes the file at the given path.
 * \param path Path to file to be deleted.
 */
void DeleteFile(QString path);

/*!
 * \brief CreateDirectory Create a new drectory with the specified path.
 * \param path Path to new directory.
 */
void CreateDirectory(QString path);

/*!
 * \brief CopyFile Copy a file.
 * \param origin The path to the original file.
 * \param destination Path to the copy of the file.
 */
void CopyFile(QString origin, QString destination);

/*!
 * \brief CopyDirectory Copy a directory and it's contents to a new destination.
 *
 * Note that this is not a recursive function: It will copy files in the root of the
 * directory, and _create_ any subdirectories found, but it will not copy the contents
 * of subdirectories.
 * \param origin Path to the original directory to be copied.
 * \param destination Path to the _parent directory_ for the copy.
 */
void CopyDirectory(QString origin, QString destination);


/*!
 * \brief GetBuildDirectoryPath Gets the absolute path to the first directory in the tree
 * that starts with build-.
 *
 * \todo Improve this.
 */
QString GetBuildDirectoryPath();

/*!
 * \brief GetProjectDirectoryPath Gets the absolute path to the project directory. Note that
 * this assumes that the root project directory contains the build directory.
 *
 * \todo Improve this.
 */
QString GetProjectDirectoryPath();

}
}

#endif // FILEHANDLING_H
