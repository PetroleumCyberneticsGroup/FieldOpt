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
#include "Utilities/settings/settings.h"
#include "Model/model.h"

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
     */
    Logger(RuntimeSettings *rts);

    /*!
     * \brief LogSettings Write a log containing some of the settings specified in the driver file.
     */
    void LogSettings(const Utilities::Settings::Settings *settings);

    /*!
     * \brief WriteVariableMap Write a CSV file with mappings from property UUID to property name.
     */
    void WritePropertyUuidNameMap(Model::Model *model);

    /*!
     * \brief LogCase Add a case to the case log.
     * \param c The case to be logged.
     * \param message An optional message that will be printed to the console.
     */
    void LogCase(const Optimization::Case *c, QString message="");

    /*!
     * \brief LogOptimizerStatus Log the current status of the optimizer.
     * \param opt Pointer to the optimizer object to log.
     * \param message An optional message that will be printed to the console.
     */
    void LogOptimizerStatus(const Optimization::Optimizer *opt, QString message="");

    /*!
     * \brief LogSimulation Log the start/stop of the simulation of a case.
     *
     * If it is the first time this method is called with a case, the start time for the
     * case will be registered. If it is the second time, the end time will be registered
     * and the log line will be written.
     * \param c The case that is being simulated.
     * \param message An optional message that will be printed to the console.
     */
    void LogSimulation(const Optimization::Case *c, QString message="");



private:
    bool verbose_; //!< Whether or not new log entries should also be printed to the console.
    QString output_dir_; //!< Directory in which the files will be written.
    QString opt_log_path_; //!< Path to the optimization log file.
    QString sim_log_path_; //!< Path to the simulation log file.
    QString cas_log_path_; //!< Path to the case log file.
    QString settings_log_path_; //!< Path to the settings log file.
    QString property_uuid_name_map_path_; //!< Path to the property uuid-name map.

    QStringList opt_header_; //!< CSV header for the optimization log
    QStringList sim_header_; //!< CSV header for the simulation log
    QStringList cas_header_; //!< CSV header for for the case log

    QHash<QUuid, QDateTime> sim_start_times_; //!< A list of start times for currently running simulations.

    void initializeCaseLog(const Optimization::Case *c);
    void initializeOptimizerLog(const Optimization::Optimizer *opt);
    void initializeSimulationLog();
    QString getTimeStampString();
    QString getTimeStampString(QDateTime t);
};

}

#endif // LOGGER_H
