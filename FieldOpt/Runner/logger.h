#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QUuid>
#include "Optimization/case.h"
#include "Optimization/optimizer.h"
#include "runtime_settings.h"
#include "Settings/settings.h"
#include "Model/model.h"
#include "Simulation/results/results.h"

namespace Runner {

/*!
 * \brief The Logger class is responsible for writing CSV logs to the disk, as well as printing
 * information to the console at runtime.
 */
class Logger
{
public:
    /*!
     * \brief Logger
     * \param rts Runtime settings
     * \param output_subdir Optional subdirectory in the output directory to write the logs in.
     */
    Logger(RuntimeSettings *rts, QString output_subdir="");


private:
    bool verbose_; //!< Whether or not new log entries should also be printed to the console.
    QString output_dir_; //!< Directory in which the files will be written.
    QString opt_log_path_; //!< Path to the optimization log file.
    QString cas_log_path_; //!< Path to the case log file.

    QStringList opt_header_; //!< CSV header for the optimization log
    QStringList cas_header_; //!< CSV header for for the case log
};

}

#endif // LOGGER_H
