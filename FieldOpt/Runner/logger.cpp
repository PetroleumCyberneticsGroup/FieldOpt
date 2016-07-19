#include "logger.h"

namespace Runner {

    Logger::Logger(RuntimeSettings *rts)
    {
        shortest_simulation_time_ = 0;
        verbose_ = rts->verbose();
        output_dir_ = rts->output_dir();
        opt_log_path_ = output_dir_ + "/log_optimization.csv";
        sim_log_path_ = output_dir_ + "/log_simulation.csv";
        cas_log_path_ = output_dir_ + "/log_cases.csv";
        run_log_path_ = output_dir_ + "/log_runner.csv";
        compdat_log_path_ = output_dir_ + "/log_compdat.txt";
        prod_data_log_path_ = output_dir_ + "/log_production_data.txt";
        settings_log_path_ = output_dir_ + "/log_settings.csv";
        property_uuid_name_map_path_ = output_dir_ + "/log_property_uuid_name_map.csv";
        QStringList log_paths = (QStringList() << cas_log_path_ << opt_log_path_ << sim_log_path_
                                               << compdat_log_path_ << prod_data_log_path_ << run_log_path_
                                               << settings_log_path_ << property_uuid_name_map_path_);

        // Delete existing logs if --force flag is on
        if (rts->overwrite_existing()) {
            for (auto path : log_paths) {
                if (Utilities::FileHandling::FileExists(path)) {
                    std::cout << "Force flag on. Deleting " << path.toStdString() << std::endl;
                    Utilities::FileHandling::DeleteFile(path);
                }
            }
        }

        initializeRunnerLog();
    }

    void Logger::LogSettings(const Utilities::Settings::Settings *settings)
    {
        Utilities::FileHandling::WriteStringToFile(settings->GetLogCsvString(), settings_log_path_);
    }

    void Logger::WritePropertyUuidNameMap(Model::Model *model)
    {
        Model::Properties::VariablePropertyContainer *properties = model->variables();

        // Write header
        Utilities::FileHandling::WriteLineToFile("UUID,name", property_uuid_name_map_path_);

        // Binary properties
        for (QUuid id : properties->GetBinaryVariables()->keys()) {
            QString line = QString("%1,%2").arg(id.toString()).arg(properties->GetBinaryVariables()->value(id)->name());
            Utilities::FileHandling::WriteLineToFile(line, property_uuid_name_map_path_);
        }

        // Continous properties
        for (QUuid id : properties->GetContinousVariables()->keys()) {
            QString line = QString("%1,%2").arg(id.toString()).arg(properties->GetContinousVariables()->value(id)->name());
            Utilities::FileHandling::WriteLineToFile(line, property_uuid_name_map_path_);
        }

        // Discrete properties
        for (QUuid id : properties->GetDiscreteVariables()->keys()) {
            QString line = QString("%1,%2").arg(id.toString()).arg(properties->GetDiscreteVariables()->value(id)->name());
            Utilities::FileHandling::WriteLineToFile(line, property_uuid_name_map_path_);
        }
    }

    void Logger::LogCase(const Optimization::Case *c, QString message)
    {
        if (!Utilities::FileHandling::FileExists(cas_log_path_))
            initializeCaseLog(c);

        // time stamp and case id
        QStringList line = (QStringList() << getTimeStampString() << c->id().toString());

        // objective function value
        try {
            QString obj_fun_val = QString("%1").arg(c->objective_function_value());
            line << "true" << obj_fun_val;
        } catch (...) {
            line << "false" << "0";
        }

        // variables
        for (QUuid bin_id : c->binary_variables().keys())
            line << bin_id.toString() << QString::number(c->binary_variables()[bin_id]);
        for (QUuid int_id : c->integer_variables().keys())
            line << int_id.toString() << QString::number(c->integer_variables()[int_id]);
        for (QUuid rea_id : c->real_variables().keys())
            line << rea_id.toString() << QString::number(c->real_variables()[rea_id]);

        if (line.size() != cas_header_.size())
            throw std::runtime_error("Case header/line length mismatch in logger.");

        Utilities::FileHandling::WriteLineToFile(line.join(","), cas_log_path_);

        if (verbose_) {
            std::cout << "Case:_____________________________________" << std::endl;
            if (message.size() > 0) std::cout << "MESSAGE: " << message.toStdString() << std::endl;
            for (int i = 0; i < cas_header_.size(); ++i) {
                std::cout << "\t" << cas_header_[i].toStdString() << ": " << line[i].toStdString() << std::endl;
            }
        }
    }

    void Logger::LogOptimizerStatus(const Optimization::Optimizer *opt, QString message)
    {
        if (!Utilities::FileHandling::FileExists(opt_log_path_))
            initializeOptimizerLog(opt);

        QStringList line = (QStringList() << getTimeStampString() << opt->GetStatusString().split(","));
        if (line.size() != opt_header_.size())
            throw std::runtime_error("Optimizer header/line length mismatch in logger. ");

        Utilities::FileHandling::WriteLineToFile(line.join(","), opt_log_path_);

        if (verbose_) {
            std::cout << "Optimizer status:_________________________" << std::endl;
            if (message.size() > 0) std::cout << "MESSAGE: " << message.toStdString() << std::endl;
            for (int i = 0; i < opt_header_.size(); ++i) {
                std::cout << "\t" << opt_header_[i].toStdString() << ": " << line[i].toStdString() << std::endl;
            }
        }
    }

    void Logger::LogSimulation(const Optimization::Case *c, bool success, QString message)
    {
        if (!Utilities::FileHandling::FileExists(sim_log_path_))
            initializeSimulationLog();

        if (!sim_start_times_.contains(c->id())) {
            sim_start_times_[c->id()] = QDateTime::currentDateTime();
            if (verbose_) {
                std::cout << "Registered simulation start for Case " << c->id().toString().toStdString() << std::endl;
                if (message.size() > 0) std::cout << "MESSAGE: " << message.toStdString() << std::endl;
            }
        }
        else {
            QDateTime start = sim_start_times_[c->id()];
            QDateTime stop = QDateTime::currentDateTime();
            int duration = (stop.toMSecsSinceEpoch() - start.toMSecsSinceEpoch())/1000;
            sim_start_times_.remove(c->id());
            QStringList line = (QStringList() << getTimeStampString(start) << getTimeStampString(stop) << QString::number(duration) << c->id().toString());

            if (line.size() != sim_header_.size())
                throw std::runtime_error("Simulation header/line length mismatch in logger.");

            Utilities::FileHandling::WriteLineToFile(line.join(","), sim_log_path_);

            if (verbose_) {
                std::cout << "Simulation ended:_________________________" << std::endl;
                if (message.size() > 0) std::cout << "MESSAGE: " << message.toStdString() << std::endl;
                for (int i = 0; i < sim_header_.size(); ++i) {
                    std::cout << "\t" << sim_header_[i].toStdString() << ": " << line[i].toStdString() << std::endl;
                }
            }

            if (success) {
                if (verbose_)
                    std::cout << "Simulation completed successfully after " << duration << " seconds." << std::endl;
                if (shortest_simulation_time_ == 0 || shortest_simulation_time_ > duration) {
                    shortest_simulation_time_ = duration;
                    if (verbose_)
                        std::cout << "New shortest simulation time recorded: " << shortest_simulation_time_ << "seconds" << std::endl;
                }
            }
            else {
                if (verbose_)
                    std::cout << "Simulation was terminated after " << duration << " seconds." << std::endl;
                timed_out_simulations_++;
            }
        }
    }

    void Logger::LogCompdat(const Optimization::Case *c, const QString compdat)
    {
        Utilities::FileHandling::WriteLineToFile(c->id().toString(), compdat_log_path_);
        Utilities::FileHandling::WriteLineToFile(compdat, compdat_log_path_);
    }

    void Logger::LogProductionData(const Optimization::Case *c, Simulation::Results::Results *results)
    {
        Utilities::FileHandling::WriteLineToFile(c->id().toString(), prod_data_log_path_);
        QString time_string = "TIME: ";
        for (double value : results->GetValueVector(Simulation::Results::Results::Property::Time)) {
            time_string.append(QString("%1, ").arg(value));
        }
        QString fopt_string = "FOPT: ";
        for (double value : results->GetValueVector(Simulation::Results::Results::Property::CumulativeOilProduction)) {
            fopt_string.append(QString("%1, ").arg(value));
        }
        QString fwpt_string = "FWPT: ";
        for (double value : results->GetValueVector(Simulation::Results::Results::Property::CumulativeWaterProduction)) {
            fwpt_string.append(QString("%1, ").arg(value));
        }
        QString fgpt_string = "FGPT: ";
        for (double value : results->GetValueVector(Simulation::Results::Results::Property::CumulativeGasProduction)) {
            fgpt_string.append(QString("%1, ").arg(value));
        }
        Utilities::FileHandling::WriteLineToFile(time_string, prod_data_log_path_);
        Utilities::FileHandling::WriteLineToFile(fopt_string, prod_data_log_path_);
        Utilities::FileHandling::WriteLineToFile(fwpt_string, prod_data_log_path_);
        Utilities::FileHandling::WriteLineToFile(fgpt_string, prod_data_log_path_);
    }

    void Logger::initializeCaseLog(const Optimization::Case *c)
    {
        cas_header_ = (QStringList() << "TimeStamp" << "CaseID" << "Evaluated" << "ObjFunVal");
        for (int i = 0; i < c->binary_variables().count(); ++i)
            cas_header_ << "BinVarID" << "BinVarVal";
        for (int i = 0; i < c->integer_variables().count(); ++i)
            cas_header_ << "IntVarID" << "IntVarVal";
        for (int i = 0; i < c->real_variables().count(); ++i)
            cas_header_ << "ReaVarID" << "ReaVarVal";

        Utilities::FileHandling::WriteStringToFile(cas_header_.join(","), cas_log_path_);
    }

    void Logger::initializeOptimizerLog(const Optimization::Optimizer *opt)
    {
        opt_header_ = (QStringList() << "TimeStamp" << opt->GetStatusStringHeader().split(","));
        Utilities::FileHandling::WriteStringToFile(opt_header_.join(","), opt_log_path_);
    }

    void Logger::initializeSimulationLog()
    {
        sim_header_ = (QStringList() << "StartTime" << "EndTime" << "Duration" << "CaseID");
        Utilities::FileHandling::WriteStringToFile(sim_header_.join(","), sim_log_path_);
    }

    QString Logger::getTimeStampString()
    {
        return QDateTime::currentDateTime().toString(Qt::ISODate);
    }

    QString Logger::getTimeStampString(QDateTime t)
    {
        return t.toString(Qt::ISODate);
    }

    void Logger::initializeRunnerLog() {
        run_header_ = (QStringList()
                << "ElapsedSecs" << "TotalCases"  << "SimulatedCases" << "BookkeepedCases" << "InvalidCases" << "TimedOutSimulations");
        start_time_ = QDateTime::currentDateTime();
        simulated_cases_ = 0;
        bookkeeped_cases_ = 0;
        invalid_cases_ = 0;
        total_cases_ = 0;
        timed_out_simulations_ = 0;
        Utilities::FileHandling::WriteLineToFile(run_header_.join(","), run_log_path_);
    }

    void Logger::LogRunnerStats() {
        QString line = QString("%1,%2,%3,%4,%5,%6")
                .arg((QDateTime::currentMSecsSinceEpoch() - start_time_.currentMSecsSinceEpoch()) / 1000.0)
                .arg(total_cases_).arg(simulated_cases_).arg(bookkeeped_cases_).arg(invalid_cases_).arg(timed_out_simulations_);
        Utilities::FileHandling::WriteLineToFile(line, run_log_path_);
        if (verbose_) {
            QString stats = QString("Runner stats:  Time elapsed: %1\n\tCases:\n\ttotal: %2 -- simulated: %3 -- bookkeeped: %4 -- invalid: %5 -- timedout: %6")
                    .arg((QDateTime::currentMSecsSinceEpoch() - start_time_.currentMSecsSinceEpoch()) / 1000.0)
                    .arg(total_cases_).arg(simulated_cases_).arg(bookkeeped_cases_).arg(invalid_cases_).arg(timed_out_simulations_);
            std::cout << stats.toStdString() << std::endl;
        }
    }

    void Logger::increment_simulated_cases() {
        simulated_cases_++;
        total_cases_++;
    }

    void Logger::increment_bookkeeped_cases() {
        bookkeeped_cases_++;
        total_cases_++;
    }

    void Logger::increment_invalid_cases() {
        invalid_cases_++;
        total_cases_++;
    }

    QString Logger::GetTimeInfoString() const {
        QString start = start_time_.toLocalTime().toString("ddd MMMM d - hh:mm::ss");
        QString end = QDateTime::currentDateTime().toLocalTime().toString("ddd MMMM d - hh:mm::ss");
        int duration_secs = (QDateTime::currentDateTime().toMSecsSinceEpoch() - start_time_.toMSecsSinceEpoch());
        int hrs  = duration_secs/3600;
        int mins = (duration_secs % 3600) / 60;
        int secs = duration_secs % 60;
        QString duration = QString("%1 hours, %2 minutes, %3 seconds.").arg(hrs).arg(mins).arg(secs);
        return QString("Start:    %1\n"
                       "End:      %2\n"
                       "Duration: %3\n").arg(start).arg(end).arg(duration);
    }

    QString Logger::GetSimInfoString() const {
        QString total       = "Total number of cases:   " + QString::number(total_cases_);
        QString simulated   = "Simulated cases:         " + QString::number(simulated_cases_);
        QString bookkeeped  = "Bookkeeped cases:        " + QString::number(bookkeeped_cases_);
        QString timedout    = "Timed out simulations:   " + QString::number(timed_out_simulations_);
        QString invalid     = "Invalid cases generated: " + QString::number(invalid_cases_);
        QString pct_invalid = "Percentage of cases invalid:         " + QString::number(float(invalid_cases_) / float(total_cases_) * 100) + "%";
        QString pct_bkpd    = "Percentage of cases bookkeeped:      " + QString::number(float(bookkeeped_cases_) / float(total_cases_-invalid_cases_) * 100) + "%";
        QString pct_to      = "Percentage of simulations timed out: " + QString::number(float(timed_out_simulations_) / float(total_cases_-invalid_cases_-bookkeeped_cases_) * 100) + "%";
        return total + "\n" + simulated + "\n" + bookkeeped + "\n" + timedout + "\n" + invalid + "\n"
               + pct_invalid + "\n" + pct_bkpd + "\n" + pct_to + "\n";
    }


}
