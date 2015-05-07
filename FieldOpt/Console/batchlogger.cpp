#include "batchlogger.h"

BatchLogger::BatchLogger(QString outputDirectoryPath, int worldSize)
{
    m_world_size = worldSize;
    m_output_directory_path = outputDirectoryPath;
    m_batch_id = 0;

    QFile file;
    QDir::setCurrent(m_output_directory_path);
    file.setFileName("batchlog_" + QString::number(m_world_size) + "p.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "BID\tBSZ\tSTART\tEND\tDUR" << "\n";
    file.flush();
    file.close();
}

void BatchLogger::newBatchStart(int batchSize)
{
    m_batch_size = batchSize;
    m_start_time = QDateTime::currentDateTime();
}

void BatchLogger::batchEnd()
{
    QFile file;
    QDir::setCurrent(m_output_directory_path);
    file.setFileName("batchlog_" + QString::number(m_world_size) + "p.txt");

    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QDateTime endTime = QDateTime::currentDateTime();
    int duration = endTime.toMSecsSinceEpoch() - m_start_time.toMSecsSinceEpoch();

    QTextStream out(&file);
    out << m_batch_id << "\t" << m_batch_size << "\t" << m_start_time.toMSecsSinceEpoch() << "\t" << endTime.toMSecsSinceEpoch() << "\t" << duration << "\n";
    file.flush();
    file.close();

    m_batch_id++;
}
