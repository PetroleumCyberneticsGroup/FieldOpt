#include "filehandling.h"
#include "filehandling_exceptions.h"
#include <QTextStream>
#include <QDir>

bool Utilities::FileHandling::FileExists(QString file_path)
{
    QFileInfo file(file_path);
    QFileInfo file_relative(file.absoluteFilePath());
    if (file.exists() && file.isFile())
        return true;
    else return file_relative.exists() && file_relative.isFile();
}

bool Utilities::FileHandling::DirectoryExists(QString directory_path)
{
    QFileInfo folder(directory_path);
    return folder.exists() && folder.isDir();
}

bool Utilities::FileHandling::ParentDirectoryExists(QString file_path)
{
    QFileInfo file(file_path);
    QDir parent_directory = file.dir();
    return parent_directory.exists();
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

    if (!string.endsWith("\n"))
        string.append("\n");

    QFile file(file_path);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream out(&file);
    out << string.toUtf8();
    file.close();
}

void Utilities::FileHandling::WriteLineToFile(QString string, QString file_path)
{
    if (!ParentDirectoryExists(file_path))
        throw DirectoryNotFoundException("Specified file's parent directory does not exist", file_path);

    if (!string.endsWith("\n"))
        string.append("\n");

    QFile file(file_path);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out << string.toUtf8();
    file.close();
}

QString Utilities::FileHandling::GetCurrentDirectoryPath()
{
    QDir path = QDir::currentPath(); // Get current directory
    return path.absolutePath();
}

void Utilities::FileHandling::DeleteFile(QString path)
{
    if (FileExists(path)) {
        QFile file(path);
        file.remove();
    }
    else throw FileNotFoundException(path);
}

bool Utilities::FileHandling::DirectoryIsEmpty(QString folder_path)
{
    if (!DirectoryExists(folder_path)) return false;
    QDir directory = QDir(folder_path);
    return directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0;
}

void Utilities::FileHandling::CopyFile(QString origin, QString destination)
{
    if (!FileExists(origin))
        throw FileNotFoundException(origin);
    QFile original(origin);
    if (!original.copy(destination))
        FileHandlingException("Failed to copy file " + origin + " to " + destination);
}

void Utilities::FileHandling::CopyDirectory(QString origin, QString destination)
{
    if (!DirectoryExists(origin))
        throw DirectoryNotFoundException("Can't find parent directory for copying: ", origin);
    if (!DirectoryExists(destination))
        throw DirectoryNotFoundException("Can't find destination (parent) directory for copying: ", destination);
    QDir original(origin);
    QFileInfoList entries = original.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsLast);

    for (auto entry : entries) {
        if (entry.isFile() && !entry.isDir())
            CopyFile(entry.absoluteFilePath(), destination+"/"+entry.fileName()); //std::cout << "FILE: " << QString().toStdString() << std::endl;
        else if (entry.isDir())
            CreateDirectory(destination+"/"+entry.fileName()); // std::cout << "FOLDER: " << QString().toStdString() << std::endl;
    }

}

void Utilities::FileHandling::CreateDirectory(QString path)
{
    if (DirectoryExists(path))
        return; // Do nothing if the directory already exists.
    QDir().mkdir(path);
}
