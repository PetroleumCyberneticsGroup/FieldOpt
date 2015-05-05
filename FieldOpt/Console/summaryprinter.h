#ifndef SUMMARYPRINTER_H
#define SUMMARYPRINTER_H

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QVector>
#include <QStringList>
#include <QDateTime>
#include <QString>
#include <optimizers/case.h>

/*!
 * \brief The SummaryPrinter class is used for printing a summary file at the end of the optimization.
 *
 * The SummaryPrinter should be initialized before optimization starts in order to get the correct run-time.
 */
class SummaryPrinter
{
private:
    QDateTime start_time;
    QDateTime end_time;
    int run_time;
    double best_result;
    QVector<int> best_int_vars;
    QVector<double> best_bool_vars;
    QVector<double> best_real_vars;
    QString output_directory_path;
    int mpi_size;

    void setData(Case* c);

public:
    SummaryPrinter(QString outputDirectoryPath, int mpiSize);
    void printSummary(Case* c);
};

#endif // SUMMARYPRINTER_H
