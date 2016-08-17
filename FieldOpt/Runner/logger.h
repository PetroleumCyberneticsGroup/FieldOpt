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
     * \param success True should be passed if the simulation was, for some reason, terminated.
     * \param message An optional message that will be printed to the console.
     */
    void LogSimulation(const Optimization::Case *c, bool success=true, QString message="");

    /*!
     * \brief LogCompdat Log the compdat section for the simulation of a case.
     */
    void LogCompdat(const Optimization::Case *c, const QString compdat);

    /*!
     * \brief LogProductionData Log the production data (e.g. production rates and cumulatives)
     * from a simulation.
     */
    void LogProductionData(const Optimization::Case *c, Simulation::Results::Results *results);

    void LogRunnerStats();
    void increment_simulated_cases(); //!< Increase the number of simulated cases by 1.
    void increment_bookkeeped_cases(); //!< Increase the number of bookkeeped cases by 1.
    void increment_invalid_cases(); //!< Increase the number of invalid cases by 1.
    void increment_timed_out_cases(); //!< Increase the number of invalid cases by 1. This should be used for parallel runners, as the automatic method wont currently work with them.
    int shortest_simulation_time() { return shortest_simulation_time_; } //!< Get the shortest recorded simulation time.

    QString GetTimeInfoString() const; //!< Get a string with information about time spent running the optimization. Intended for printing at end of run.
    QString GetSimInfoString() const; //!< Get a string with simulation stats. Intended for printing at end of optimization run.

    QList<int> simulator_execution_times() const { return simulator_execution_times_; }

private:
    bool verbose_; //!< Whether or not new log entries should also be printed to the console.
    QString output_dir_; //!< Directory in which the files will be written.
    QString opt_log_path_; //!< Path to the optimization log file.
    QString sim_log_path_; //!< Path to the simulation log file.
    QString cas_log_path_; //!< Path to the case log file.
    QString run_log_path_; //!< Path to the runner log file.
    QString compdat_log_path_; //!< Path to the COMPDAT log file.
    QString prod_data_log_path_; //!< Path to the production data log file.
    QString settings_log_path_; //!< Path to the settings log file.
    QString property_uuid_name_map_path_; //!< Path to the property uuid-name map.

    QStringList opt_header_; //!< CSV header for the optimization log
    QStringList sim_header_; //!< CSV header for the simulation log
    QStringList cas_header_; //!< CSV header for for the case log
    QStringList run_header_; //!< CSV header for the runner log

    int simulated_cases_;
    int bookkeeped_cases_;
    int invalid_cases_;
    int total_cases_;
    int timed_out_simulations_;

    QHash<QUuid, QDateTime> sim_start_times_; //!< A list of start times for currently running simulations.
    QList<int> simulator_execution_times_;
    QDateTime start_time_; //!< The start time for FieldOpt
    int shortest_simulation_time_; //!< The shortest recorded simulation time.

    void initializeCaseLog(const Optimization::Case *c);
    void initializeOptimizerLog(const Optimization::Optimizer *opt);
    void initializeSimulationLog();
    void initializeRunnerLog();
    QString getTimeStampString();
    QString getTimeStampString(QDateTime t);
};

}

#endif // LOGGER_H
