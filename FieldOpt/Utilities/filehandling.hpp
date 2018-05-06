/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef FILEHANDLING_H
#define FILEHANDLING_H

// ---------------------------------------------------------
// QT
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>

// ---------------------------------------------------------
// STD
#include <stdexcept>
#include <iostream>
#include <vector>

// ---------------------------------------------------------
// BOOST
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

// ---------------------------------------------------------
// FIELDOPT
#include "colors.hpp"
#include "debug.hpp"

// ---------------------------------------------------------
namespace Utilities {
namespace FileHandling {

using std::cout;
using std::vector;
// using std::endl;
using std::runtime_error;

// =========================================================
/*!
 * \brief FileExists Checks whether or not
 * a file exists at the specified path.
 *
 * \param file_path Path to a file that
 * may or may not exist.
 *
 * \param verbose Whether the path being
 * checked should be printed.
 *
 * \return True if a file exists at the
 * specified path, otherwise false.
 */
inline bool FileExists(QString file_path,
                       vector<int> verbv = vector<int>(11,0)) {

  // -------------------------------------------------------
  QFileInfo file(file_path);
  QFileInfo file_relative(file.absoluteFilePath());

  // -------------------------------------------------------
  if (file.exists() && file.isFile()) {

    // -----------------------------------------------------
    if (verbv[10] > 3) // idx:10 -> uti
      cout << fstr("[uti]File exists at path:",10)
           << file_path.toStdString() << endl;
    return true;

  } else if (file_relative.exists() && file_relative.isFile()) {

    // -----------------------------------------------------
    if (verbv[10] > 3) // idx:10 -> uti
      cout << fstr("[uti]File exists at relative path:",10)
           << file_path.toStdString() << endl;
    return true;

  } else {

    // -----------------------------------------------------
    if (verbv[10] > 3) // idx:10 -> uti
      cout << fstr("[uti]File does not exists:",10)
           << file_path.toStdString() << endl;
    return false;
  }
}

// =========================================================
/*!
 * \brief DirectoryExists Checks whether or
 * not a folder exists at the specified path.
 *
 * \param folder_path Path to a folder
 * that may or may not exist.
 *
 * \param verbose Whether the path being
 * checked should be printed.
 *
 * \return True if a folder exists at
 * the specified path, otherwise false.
 */
inline bool DirectoryExists(QString directory_path,
                            vector<int> verbv = vector<int>(11,0)) {

  // -------------------------------------------------------
  QFileInfo folder(directory_path);
  if (folder.exists() && folder.isDir()) {

    // -----------------------------------------------------
    if (verbv[10] > 3) // idx:10 -> uti
      cout << fstr("[uti]Directory exists at path:",10)
           << directory_path.toStdString() << endl;
    return true;

  } else {

    // -----------------------------------------------------
    if (verbv[10] > 3) // idx:10 -> uti
      cout << fstr("[uti]Directory does not exists at path:",10)
           << directory_path.toStdString() << endl;
    return false;
  }
}

// =========================================================
/*!
 * \brief DirectoryIsEmpty Check whether
 * or not a directory is empty.
 *
 * \param folder_path Path a folder to check.
 *
 * \return True if the directory is empty,
 * otherwise false.
 */
inline bool DirectoryIsEmpty(QString folder_path) {

  // -------------------------------------------------------
  if (!DirectoryExists(folder_path)) {
    return false;
  }

  // -------------------------------------------------------
  QDir directory = QDir(folder_path);
  auto dir = directory.entryInfoList(
      QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0;

  return dir;
}

// =========================================================
/*!
 * \brief ParentDirectoryExists Checks whether
 * a specified file's parent directory exists.
 *
 * \param file_path Path a file (the file itself
 * does not have to exist).
 *
 * \return True if the parent directory exists,
 * otherwise false.
 */
inline bool ParentDirectoryExists(QString file_path) {

  // -------------------------------------------------------
  QFileInfo file(file_path);
  QDir parent_directory = file.dir();
  return parent_directory.exists();
}

// =========================================================
/*!
 * \brief ReadFileToStringList Reads the contents of
 * a file and stores it as a string list where each
 * element is a line in the file.
 *
 * \param file_path The file to create a list from.
 *
 * \return List where each element is a line in the file.
 */
inline QStringList *ReadFileToStringList(QString file_path) {

  // -------------------------------------------------------
  QStringList *string_list = new QStringList();
  QFile file(file_path);

  // -------------------------------------------------------
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw runtime_error(
        "File not found: " + file_path.toStdString());
  }

  // -------------------------------------------------------
  QTextStream text_stream(&file);

  // -------------------------------------------------------
  while (true) {

    QString line = text_stream.readLine();
    if (line.isNull()) {
      break;
    } else {
      string_list->append(line);
    }
  }

  // -------------------------------------------------------
  file.close();
  return string_list;
}

// =========================================================
/*!
 * \brief WriteStringToFile Write a string to
 *  a file. Removes existing file contents.
 *
 * If the string does not end with a newline,
 * it will be added.
 *
 * \param string The string to be written.
 *
 * \param file_path Path to the file
 * to write the string into.
 */
inline void WriteStringToFile(QString string,
                              QString file_path) {

  // -------------------------------------------------------
  if (!ParentDirectoryExists(file_path))
    throw runtime_error(
        "File's parent directory not found: "
            + file_path.toStdString());

  if (!string.endsWith("\n"))
    string.append("\n");

  // -------------------------------------------------------
  QFile file(file_path);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  QTextStream fout(&file);
  fout << string.toUtf8() << endl;
  file.close();
}

// =========================================================
/*!
 * \brief WriteLineToFile Append a string to a file.
 *
 * If the string does not end with a newline,
 * it will be added.
 *
 * \param string The string/line to be written.
 *
 * \param file_path The file to write the string/line to.
 */
inline void WriteLineToFile(QString string,
                            QString file_path) {

  // -------------------------------------------------------
  if (!ParentDirectoryExists(file_path))
    throw runtime_error(
        "File's parent directory not found: "
            + file_path.toStdString());

  // -------------------------------------------------------
  if (!string.endsWith("\n"))
    string.append("\n");

  // -------------------------------------------------------
  QFile file(file_path);
  file.open(QIODevice::Append);
  QTextStream fout(&file);
  fout << string.toUtf8();
  file.close();
}

// =========================================================
/*!
 * \brief DeleteFile Deletes the file at the given path.
 *
 * \param path Path to file to be deleted.
 */
inline void DeleteFile(QString path) {

  // -------------------------------------------------------
  if (FileExists(path)) {
    QFile file(path);
    file.remove();

  } else {
    throw runtime_error("File not found: " + path.toStdString());
  }
}

// =========================================================
/*!
 * \brief CreateDirectory Create a new
 * drectory with the specified path.
 *
 * \param path Path to new directory.
 */
inline void CreateDirectory(QString path) {

  // -------------------------------------------------------
  if (DirectoryExists(path)) {
    return; // Do nothing if the directory already exists.
  }

  // -------------------------------------------------------
  QDir().mkpath(path);
  // QDir().mkdir(path);
}

// =========================================================
/*!
 * \brief CopyFile Copy a file.
 * \param origin The path to the original file.
 * \param destination Path to the copy of the file.
 * \param overwrite Overwrite existing file.
 */
inline void CopyFile(QString origin,
                     QString destination,
                     bool overwrite) {

  // -------------------------------------------------------
  if (!FileExists(origin)) {
    throw runtime_error(
        "Error copying. Original file not found: "
            + origin.toStdString());
  }

  // -------------------------------------------------------
  if (overwrite) {

    boost::filesystem::copy_file(
        origin.toStdString(),
        destination.toStdString(),
        boost::filesystem::copy_option::overwrite_if_exists);

  } else {

    boost::filesystem::copy_file(origin.toStdString(),
                                 destination.toStdString());
  }
}

// =========================================================
/*!
 * \brief CopyDirectory Copy a directory
 * and it's contents to a new destination.
 *
 * Note that this is not a recursive function: It will
 * copy files in the root of the directory, and _create_
 * any subdirectories found, but it will not copy the
 * contents of subdirectories.
 *
 * \param origin Path to the original directory to be copied.
 * \param destination Path to the _parent directory_ for the copy.
 */
inline void CopyDirectory(QString origin,
                          QString destination,
                          vector<int> verbv = vector<int>(11,0)) {

  // -------------------------------------------------------
  if (!DirectoryExists(origin)) {
    throw runtime_error(
        "Can't find parent directory for copying: "
            + origin.toStdString());
  }

  // -------------------------------------------------------
  if (!DirectoryExists(destination)) {
    throw runtime_error(
        "Can't find destination directory for copying: "
            + destination.toStdString());
  }

  // -------------------------------------------------------
  QDir original(origin);
  QFileInfoList entries =
      original.entryInfoList(QDir::AllEntries
                                 | QDir::NoDotAndDotDot,
                             QDir::DirsLast);

  for (auto entry : entries) {

    // -----------------------------------------------------
    if (entry.isFile() && !entry.isDir()) {

      // ---------------------------------------------------
      CopyFile(entry.absoluteFilePath(),
               destination + "/" + entry.fileName(), true);

      // ---------------------------------------------------
      if (verbv[10] > 3) // idx:10 -> uti
        cout << fstr("[uti]Copying FILE::",10)
             << entry.fileName().toStdString() << endl;

    } else if (entry.isDir()) {

      // ---------------------------------------------------
      CreateDirectory(destination + "/" + entry.fileName());

      // ---------------------------------------------------
      if (verbv[10] > 3) // idx:10 -> uti
        cout << fstr("[uti]Copying FOLDER::",10)
             << entry.fileName().toStdString() << endl;

      // ---------------------------------------------------
      CopyDirectory(entry.absoluteFilePath(),
                    destination + "/" + entry.fileName(),
                    verbv);
    }
  }
}

// =========================================================
/*!
 * \brief GetCurrentDirectoryPath Gets the
 * absolute path to the current directory.
 *
 * \todo Improve this.
 */
inline QString GetCurrentDirectoryPath() {

  // Get current directory
  QDir path = QDir::currentPath();
  return path.absolutePath();
}

// =========================================================
/*!
 * \brief GetAbsoluteFilePath Gets absolute path of file.
 *
 * \param file (relative) path to file
 */
inline QString GetAbsoluteFilePath(QString file) {

  QFileInfo fileInfo(file);
  return fileInfo.absoluteFilePath();
}

// =========================================================
/*!
 * \brief
 *
 * \param
 */
inline void ThrowRuntimeError(std::string error_msg) {

  std::cout << "RUNTIME ERROR: " << error_msg << std::endl;
  throw runtime_error(error_msg);
}

}
}

#endif // FILEHANDLING_H
