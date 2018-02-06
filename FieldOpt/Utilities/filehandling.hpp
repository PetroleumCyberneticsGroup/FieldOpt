#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "colors.hpp"

namespace Utilities {
namespace FileHandling {

/*!
 * \brief FileExists Checks whether or not a file exists at the specified path.
 * \param file_path Path to a file that may or may not exist.
 * \return True if a file exists at the specified path, otherwise false.
 */
inline bool FileExists(QString file_path)
{
    QFileInfo file(file_path);
    QFileInfo file_relative(file.absoluteFilePath());
    if (file.exists() && file.isFile())
        return true;
    else return file_relative.exists() && file_relative.isFile();
}

/*!
 * \brief DirectoryExists Checks whether or not a folder exists at the specified path.
 * \param folder_path Path to a folder that may or may not exist.
 * \return True if a folder exists at the specified path, otherwise false.
 */
inline bool DirectoryExists(QString directory_path)
{
    QFileInfo folder(directory_path);
    return folder.exists() && folder.isDir();
}


/*!
 * \brief DirectoryIsEmpty Check whether or not a directory is empty.
 * \param folder_path Path a folder to check.
 * \return True if the directory is empty, otherwise false.
 */
inline bool DirectoryIsEmpty(QString folder_path)
{
    if (!DirectoryExists(folder_path)) return false;
    QDir directory = QDir(folder_path);
    return directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0;
}

/*!
 * \brief ParentDirectoryExists Checks whether a specified file's parent directory exists.
 * \param file_path Path a file (the file itself does not have to exist).
 * \return True if the parent directory exists, otherwise false.
 */
inline bool ParentDirectoryExists(QString file_path)
{
    QFileInfo file(file_path);
    QDir parent_directory = file.dir();
    return parent_directory.exists();
}

/*!
 * \brief ReadFileToStringList Reads the contents of a file and stores it as
 * a string list where each element is a line in the file.
 * \param file_path The file to create a list from.
 * \return List where each element is a line in the file.
 */
inline QStringList *ReadFileToStringList(QString file_path)
{
    QStringList *string_list = new QStringList();
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("File not found: " + file_path.toStdString());

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

/*!
 * \brief WriteStringToFile Write a string to a file. Removes existing file contents.
 *
 * If the string does not end with a newline, it will be added.
 * \param string The string to be written.
 * \param file_path Path to the file to write the string into.
 */
inline void WriteStringToFile(QString string, QString file_path)
{
    if (!ParentDirectoryExists(file_path))
        throw std::runtime_error("File's parent directory not found: " + file_path.toStdString());

    if (!string.endsWith("\n"))
        string.append("\n");

    QFile file(file_path);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream out(&file);
    out << string.toUtf8() << endl;
    file.close();
}

/*!
 * \brief WriteLineToFile Append a string to a file.
 *
 * If the string does not end with a newline, it will be added.
 * \param string The string/line to be written.
 * \param file_path The file to write the string/line to.
 */
inline void WriteLineToFile(QString string, QString file_path)
{
    if (!ParentDirectoryExists(file_path))
        throw std::runtime_error("File's parent directory not found: " + file_path.toStdString());

    if (!string.endsWith("\n"))
        string.append("\n");

    QFile file(file_path);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out << string.toUtf8();
    file.close();
}

/*!
 * \brief DeleteFile Deletes the file at the given path.
 * \param path Path to file to be deleted.
 */
inline void DeleteFile(QString path)
{
    if (FileExists(path)) {
        QFile file(path);
        file.remove();
    }
    else throw std::runtime_error("File not found: " + path.toStdString());
}

/*!
 * \brief CreateDirectory Create a new drectory with the specified path.
 * \param path Path to new directory.
 */
inline void CreateDirectory(QString path)
{
    if (DirectoryExists(path))
        return; // Do nothing if the directory already exists.
    QDir().mkdir(path);
}

/*!
 * \brief CopyFile Copy a file.
 * \param origin The path to the original file.
 * \param destination Path to the copy of the file.
 * \param overwrite Overwrite existing file.
 */
inline void CopyFile(QString origin, QString destination, bool overwrite)
{
    if (!FileExists(origin))
        throw std::runtime_error("Error copying. Original file not found: " + origin.toStdString());

    if (overwrite)
        boost::filesystem::copy_file(origin.toStdString(),
                                     destination.toStdString(),
                                     boost::filesystem::copy_option::overwrite_if_exists);
    else
        boost::filesystem::copy_file(origin.toStdString(), destination.toStdString());
}

/*!
 * \brief CopyDirectory Copy a directory and it's contents to a new destination.
 *
 * Note that this is not a recursive function: It will copy files in the root of the
 * directory, and _create_ any subdirectories found, but it will not copy the contents
 * of subdirectories.
 * \param origin Path to the original directory to be copied.
 * \param destination Path to the _parent directory_ for the copy.
 */
inline void CopyDirectory(QString origin, QString destination)
{
    if (!DirectoryExists(origin))
        throw std::runtime_error("Can't find parent directory for copying: " + origin.toStdString());
    if (!DirectoryExists(destination))
        throw std::runtime_error("Can't find destination (parent) directory for copying: " + destination.toStdString());
    QDir original(origin);
    QFileInfoList entries = original.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsLast);

    for (auto entry : entries) {
        if (entry.isFile() && !entry.isDir())
            CopyFile(entry.absoluteFilePath(), destination+"/"+entry.fileName(), true); //std::cout << "FILE: " << QString().toStdString() << std::endl;
        else if (entry.isDir())
            CreateDirectory(destination+"/"+entry.fileName()); // std::cout << "FOLDER: " << QString().toStdString() << std::endl;
    }
}

/*!
 * \brief GetCurrentDirectoryPath Gets the absolute path to the current directory.
 *
 * \todo Improve this.
 */
inline QString GetCurrentDirectoryPath()
{
    QDir path = QDir::currentPath(); // Get current directory
    return path.absolutePath();
}

/*!
 * \brief GetAbsoluteFilePath Gets absolute path of file.
 *
 * \param file (relative) path to file
 */
inline QString GetAbsoluteFilePath(QString file)
{
    QFileInfo fileInfo(file);
    return fileInfo.absoluteFilePath();
}

/*!
 * \brief GetAbsoluteFilePath Gets absolute path of file.
 *
 * \param file (relative) path to file
 */
inline void ThrowRuntimeError(std::string error_msg)
{
    std::cout << "RUNTIME ERROR: " << error_msg << std::endl;
    throw std::runtime_error(error_msg);
}

}
}

#endif // FILEHANDLING_H
