#ifndef RESULTSLOGGER_H
#define RESULTSLOGGER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <iostream>

#include "Library/optimizers/case.h"
#include "Library/model/model.h"
#include "transferobjects/perturbation.h"
#include "transferobjects/result.h"

/*!
 * \brief The Logger class is used to keep track of results from simulations, as well
 * as timestamps. The results are written to to a file at the end of the optimization.
 * A Logger object should only be instantiated by the root process.
 */
class ResultsLogger
{
private:
    QStringList header;           //!< The headers to be written to the results file.
    QList<QStringList> lines;     //!< A list of lines to be written to the results file.
    QString output_directory_path;  //!< Path to the directory in which to write the results file.
    int mpi_size;  //!< The total number of MPI processes. Appended to the file name.

public:
    /*!
     * \brief Logger default constructor.
     * \param outputDirectoryPath The directory in which to write the log file.
     * \param mpiSize The number of mpi processes. Appended to the results file name.
     * \param m The Model used in the simulation. Used to create the header.
     */
    ResultsLogger(QString outputDirectoryPath, int mpiSize, Model* m);


    /*!
     * \brief Add an entry to the log.
     * \param p The perturbation to be added.
     * \param r The result of the evaluation.
     * \param evaluationTime The (in miliseconds) time it took to evaluate the case.
     */
    void addEntry(Perturbation* p, Result* r, int evaluationTime);


    /*!
     * \brief Write the log into the output directory passed in the constructor. The file will be named results.txt.
     */
    void writeLog();
    QString getOutput_directory_path() const;
};

#endif // RESULTSLOGGER_H
