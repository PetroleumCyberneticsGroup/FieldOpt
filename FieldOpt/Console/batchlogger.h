#ifndef BATCHLOGGER_H
#define BATCHLOGGER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <iostream>

class BatchLogger
{
private:
    int m_world_size;
    int m_batch_id;
    int m_batch_size;
    QString m_output_directory_path;
    QDateTime m_start_time;

public:
    BatchLogger(QString outputDirectoryPath, int worldSize);
    void newBatchStart(int batchSize);
    void batchEnd();
};

#endif // BATCHLOGGER_H
