#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>

namespace Utilities {
namespace FileHandling {

/*!
 * \brief FileExists Checks whether or not a file exists at the specified path.
 * \param file_path Path to a file that may or may not exist.
 * \param verbose Whether the path being checked should be printed.
 * \return True if a file exists at the specified path, otherwise false.
 */
inline bool FileExists(const QString &file_path, const bool &verbose=false) {
    QFileInfo file(file_path);
    QFileInfo file_relative(file.absoluteFilePath());
    if (file.exists() && file.isFile()) {
        if (verbose) std::cout << "File exists at path: " << file_path.toStdString() << std::endl;
        return true;
    }
    else if (file_relative.exists() && file_relative.isFile()) {
        if (verbose) std::cout << "File exists at relative path: " << file_path.toStdString() << std::endl;
        return true;
    }
    else {
        if (verbose) std::cout << "File does not exists: " << file_path.toStdString() << std::endl;
        return false;
    }
}

/*!
 * Overload of the FileExists(QString, bool) function. Creates a
 * QString from the std string and calls the other function.
 */
inline bool FileExists(const std::string file_path, const bool verbose=false) {
    return FileExists(QString::fromStdString(file_path), verbose);
}

/*!
 * \brief DirectoryExists Checks whether or not a folder exists at the specified path.
 * \param folder_path Path to a folder that may or may not exist.
 * \param verbose Whether the path being checked should be printed.
 * \return True if a folder exists at the specified path, otherwise false.
 */
inline bool DirectoryExists(const QString &directory_path, const bool &verbose=false) {
    QFileInfo folder(directory_path);
    if (folder.exists() && folder.isDir()) {
        if (verbose) std::cout << "Directory exists at path: " << directory_path.toStdString() << std::endl;
        return true;
    }
    else {
        if (verbose) std::cout << "Directory does not exists at path: " << directory_path.toStdString() << std::endl;
        return false;
    }
}

/*!
 * Overload of the DirectoryExists(QString, bool) function. Creates a
 * QString from the std string and calls the other function.
 */
inline bool DirectoryExists(const std::string &directory_path, const bool &verbose=false) {
    return DirectoryExists(QString::fromStdString(directory_path), verbose);
}


/*!
 * \brief DirectoryIsEmpty Check whether or not a directory is empty.
 * \param folder_path Path a folder to check.
 * \return True if the directory is empty, otherwise false.
 */
inline bool DirectoryIsEmpty(const QString &folder_path) {
    if (!DirectoryExists(folder_path)) return false;
    QDir directory = QDir(folder_path);
    return directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0;
}

inline bool DirectoryIsEmpty(const std::string &folder_path) {
    return DirectoryIsEmpty(QString::fromStdString(folder_path));
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
inline QStringList *ReadFileToStringList(const QString &file_path)
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

inline std::vector<std::string> ReadFileToStdStringList(const std::string &filepath) {
    auto qt_string_list = ReadFileToStringList(QString::fromStdString(filepath));
    auto stringlist = std::vector<std::string>(qt_string_list->size());

    for (int i = 0; i < qt_string_list->size(); ++i) {
        stringlist[i] = qt_string_list->at(i).toStdString();
    }
    qt_string_list->clear();
    delete qt_string_list;

    return stringlist;
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

inline void CreateDirectory(std::string path) {
    CreateDirectory(QString::fromStdString(path));
}

/*!
 * Get the name of a file from a path (i.e. delete everyting up to
 * and including the final /).
 * @param file_path Path to a file
 * @return Name of a file, including extension.
 */
inline std::string FileName(const std::string file_path) {
    std::vector<std::string> parts;
    boost::split(parts, file_path, boost::is_any_of("/"), boost::token_compress_on);
    return parts.back();
}

/*!
 * Get the name of a file's parent directory.
 * @param file_path Path to a file.
 * @return Name of a directory.
 */
inline std::string ParentDirectoryName(const std::string file_path) {
    std::vector<std::string> parts;
    boost::split(parts, file_path, boost::is_any_of("/"), boost::token_compress_on);
    return parts[parts.size() - 2];
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
inline void CopyDirectory(QString origin, QString destination, bool verbose=false)
{
    if (!DirectoryExists(origin))
        throw std::runtime_error("Can't find parent directory for copying: " + origin.toStdString());
    if (!DirectoryExists(destination))
        throw std::runtime_error("Can't find destination (parent) directory for copying: " + destination.toStdString());
    QDir original(origin);
    QFileInfoList entries = original.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsLast);

    for (auto entry : entries) {
        if (entry.isFile() && !entry.isDir()) {
            CopyFile(entry.absoluteFilePath(), destination + "/" + entry.fileName(), true);
            if (verbose) std::cout << "Copying FILE: " << entry.fileName().toStdString() << std::endl;
        }
        else if (entry.isDir()) {
            CreateDirectory(destination + "/" + entry.fileName());
            if(verbose) std::cout << "Copying FOLDER: " << entry.fileName().toStdString() << std::endl;
            CopyDirectory(entry.absoluteFilePath(), destination + "/" + entry.fileName(), verbose);
        }
    }
}

inline void CopyDirectory(std::string origin, std::string destination, bool verbose=false) {
    CopyDirectory(QString::fromStdString(origin), QString::fromStdString(destination), verbose);
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
inline QString GetAbsoluteFilePath(const QString &file) {
    QFileInfo fileInfo(file);
    return fileInfo.absoluteFilePath();
}

inline std::string GetAbsoluteFilePath(const std::string &file) {
    return GetAbsoluteFilePath(QString::fromStdString(file)).toStdString();
}

/*!
 * Get the path to a file's parent directory (i.e. remove everyting
 * after the final slash)
 */
inline QString GetParentDirectoryPath(const QString &file_path) {
    QStringList parts = file_path.split("/");
    parts.removeLast();
    return parts.join("/");
}

inline std::string GetParentDirectoryPath(const std::string &file_path) {
    return GetParentDirectoryPath(QString::fromStdString(file_path)).toStdString();
}

inline double new_stod(const std::string &s) {
    std::stringstream ss(s);
    double d;
    ss >> d;
    return d;
}

inline std::vector<double> LoadCSVFile_1DArray(const std::string &file_path) {

    std::ifstream data(file_path);

    std::vector<double> array;

    std::string line;
    while(std::getline(data, line))
    {
        array.push_back(new_stod(line));
    }

    return array;
}

inline std::vector<std::vector<double>> LoadCSVFile_2DArray(const std::string &file_path) {

    std::ifstream data(file_path);

    std::vector<std::vector<double>> array;

    std::string line;
    while(std::getline(data, line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<double> temp;
        while(std::getline(lineStream, cell, ','))
        {
            temp.push_back(new_stod(cell));
        }
        array.push_back(temp);
    }

    return array;
}

}
}

#endif // FILEHANDLING_H
