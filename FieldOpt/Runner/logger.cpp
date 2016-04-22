#include "logger.h"
#include "Utilities/file_handling/filehandling.h"
#include <iostream>

namespace Runner {

Logger::Logger(RuntimeSettings *rts)
{
    verbose_ = rts->verbose();
    output_dir_ = rts->output_dir();
    opt_log_path_ = output_dir_ + "/log_optimization.csv";
    sim_log_path_ = output_dir_ + "/log_simulation.csv";
    cas_log_path_ = output_dir_ + "/log_cases.csv";
    settings_log_path_ = output_dir_ + "/log_settings.csv";
    property_uuid_name_map_path_ = output_dir_ + "/log_property_uuid_name_map.csv";

    // Delete existing logs if --force flag is on
    if (rts->overwrite_existing()) {
        foreach (auto path, (QStringList() << cas_log_path_ << opt_log_path_ << sim_log_path_ << settings_log_path_ << property_uuid_name_map_path_)) {
            if (Utilities::FileHandling::FileExists(path)) {
                std::cout << "Force flag on. Deleting " << path.toStdString() << std::endl;
                Utilities::FileHandling::DeleteFile(path);
            }
        }
    }
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
    foreach (QUuid id, properties->GetBinaryVariables()->keys()) {
        QString line = QString("%1,%2").arg(id.toString()).arg(properties->GetBinaryVariables()->value(id)->name());
        Utilities::FileHandling::WriteLineToFile(line, property_uuid_name_map_path_);
    }

    // Continous properties
    foreach (QUuid id, properties->GetContinousVariables()->keys()) {
        QString line = QString("%1,%2").arg(id.toString()).arg(properties->GetContinousVariables()->value(id)->name());
        Utilities::FileHandling::WriteLineToFile(line, property_uuid_name_map_path_);
    }

    // Discrete properties
    foreach (QUuid id, properties->GetDiscreteVariables()->keys()) {
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
    foreach (QUuid bin_id, c->binary_variables().keys())
        line << bin_id.toString() << QString::number(c->binary_variables()[bin_id]);
    foreach (QUuid int_id, c->integer_variables().keys())
        line << int_id.toString() << QString::number(c->integer_variables()[int_id]);
    foreach (QUuid rea_id, c->real_variables().keys())
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

void Logger::LogSimulation(const Optimization::Case *c, QString message)
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
    }
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
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
}

QString Logger::getTimeStampString(QDateTime t)
{
    return t.toString(Qt::ISODate);
}

}
