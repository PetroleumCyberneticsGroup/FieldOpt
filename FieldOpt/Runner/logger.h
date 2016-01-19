#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QUuid>
#include "Optimization/case.h"
#include "Optimization/optimizer.h"

namespace Runner {
class Logger
{
public:
    /*!
     * \brief Logger
     * \param output_dir The directory the logs should be written to.
     * \param verbose Whether or not new log entries should also be printed to the console.
     */
    Logger(QString output_dir, bool verbose);

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
    QString output_dir_;
    QString opt_log_path_;
    QString sim_log_path_;
    QString cas_log_path_;

    QStringList opt_header_;
    QStringList sim_header_;
    QStringList cas_header_;

    QHash<QUuid, QDateTime> sim_start_times_; //!< A list of start times for currently running simulations.

    void initializeCaseLog(const Optimization::Case *c);
    void initializeOptimizerLog(const Optimization::Optimizer *opt);
    void initializeSimulationLog();
    QString getTimeStampString();
    QString getTimeStampString(QDateTime t);
};

}

#endif // LOGGER_H
