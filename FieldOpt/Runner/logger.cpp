/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <iomanip>
#include <QtCore/QJsonDocument>
#include "logger.h"
#include "Utilities/time.hpp"


Logger::Logger(Runner::RuntimeSettings *rts,
               QString output_subdir,
               bool write_logs)
{
    write_logs_ = write_logs;
    verbose_ = rts->verbosity_level();
    output_dir_ = rts->output_dir();
    if (output_subdir.length() > 0)
        output_dir_ = output_dir_ + "/" + output_subdir + "/";
    opt_log_path_ = output_dir_ + "/log_optimization.csv";
    cas_log_path_ = output_dir_ + "/log_cases.csv";
    ext_log_path_ = output_dir_ + "/log_extended.json";
    QStringList log_paths = (QStringList() << cas_log_path_ << opt_log_path_ << ext_log_path_);

    // Delete existing logs if --force flag is on
    if (rts->overwrite_existing()) {
        for (auto path : log_paths) {
            if (Utilities::FileHandling::FileExists(path)) {
                cout << "Force flag on. Deleting " << path.toStdString() << endl;
                Utilities::FileHandling::DeleteFile(path);
            }
        }
    }

    // Write headers
    if (write_logs_) {
        Utilities::FileHandling::WriteLineToFile(cas_log_header_, cas_log_path_);
        Utilities::FileHandling::WriteLineToFile(opt_log_header_, opt_log_path_);

        // Create base JSON document
        QJsonObject json_base;
        json_base.insert("Cases", QJsonArray());
        QJsonDocument json_doc = QJsonDocument(json_base);
        QFile json_file(ext_log_path_);
        json_file.open(QFile::WriteOnly);
        json_file.write(json_doc.toJson(QJsonDocument::Indented));
        json_file.close();
    }
}
void Logger::AddEntry(Loggable *obj) {
    switch (obj->GetLogTarget()) {
        case Loggable::LogTarget::LOG_CASE: logCase(obj); break;
        case Loggable::LogTarget::LOG_OPTIMIZER: logOptimizer(obj); break;
        case Loggable::LogTarget::LOG_EXTENDED: logExtended(obj); break;
    }
}

void Logger::logCase(Loggable *obj) {
    stringstream entry;
    entry << setw(cas_log_col_widths_["TimeSt"]) << timestamp_string() << " ,";
    entry << setw(cas_log_col_widths_["EvalSt"]) << obj->GetState()["EvalSt"] << " ,";
    entry << setw(cas_log_col_widths_["ConsSt"]) << obj->GetState()["ConsSt"] << " ,";
    entry << setw(cas_log_col_widths_["ErrMsg"]) << obj->GetState()["ErrMsg"] << " ,";
    entry << setw(cas_log_col_widths_["SimDur"]) << timespan_string(obj->GetValues()["SimDur"][0]) << " , ";
    entry.precision(6);
    entry << setw(cas_log_col_widths_["OFnVal"]) << scientific << obj->GetValues()["OFnVal"][0] << " ,";
    entry << setw(cas_log_col_widths_["CaseId"]) << obj->GetId().toString().toStdString();
    string str = entry.str();
    if (write_logs_) {
        Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), cas_log_path_);
    }
    return;
}
void Logger::logOptimizer(Loggable *obj) {
    stringstream entry;
    entry << setw(opt_log_col_widths_["TimeSt"]) << timestamp_string() << " ,";
    entry << setw(opt_log_col_widths_["TimeEl"]) << timespan_string(obj->GetState()["TimeEl"][0]) << " , ";
    entry.precision(0);
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["IterNr"]) << obj->GetValues()["IterNr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["TotlNr"]) << obj->GetValues()["TotlNr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["EvalNr"]) << obj->GetValues()["EvalNr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["BkpdNr"]) << obj->GetValues()["BkpdNr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["TimONr"]) << obj->GetValues()["TimONr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["FailNr"]) << obj->GetValues()["FailNr"][0] << " , ";
    entry << fixed << setfill('0') << setw(opt_log_col_widths_["InvlNr"]) << obj->GetValues()["InvlNr"][0] << " , ";
    entry.precision(6);
    entry << setw(opt_log_col_widths_["CBOFnV"]) << scientific << obj->GetValues()["CBOFnV"][0] << " , ";
    entry.precision(0);
    entry << obj->GetId().toString().toStdString();
    string str = entry.str();
    if (write_logs_) {
        Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), opt_log_path_);
    }
    return;
}
void Logger::logExtended(Loggable *obj) {
    QJsonObject new_entry;

    // UUID
    new_entry.insert("UUID", obj->GetId().toString());

    // Compdat string
    new_entry.insert("COMPDAT", QString::fromStdString(obj->GetState()["COMPDAT"]));

    // Variable values
    QJsonArray vars;
    for (auto const a : obj->GetValues()) {
        if(a.first.compare(0, 4, "Var#") == 0) {
            QJsonObject var;
            var.insert(QString::fromStdString(a.first), a.second[0]);
            vars.append(var);
        }
    }
    new_entry.insert("Variables", vars);

    // Production data
    QJsonArray prod;
    for (auto const a : obj->GetValues()) {
        if(a.first.compare(0, 4, "Res#") == 0) {
            QJsonObject data;
            QJsonArray prod_vector;
            for (int i = 0; i < a.second.size(); ++i) {
                prod_vector.append(a.second[i]);
            }
            data.insert(QString::fromStdString(a.first), prod_vector);
            prod.append(data);
        }
    }
    new_entry.insert("ProductionData", prod);

    if (write_logs_) {
        // Open existing document
        QFile json_file(ext_log_path_);

        // First validating existing structure
        json_file.open(QFile::ReadWrite);
        QByteArray json_data = json_file.readAll();
        QJsonObject json_obj = QJsonDocument::fromJson(json_data).object();
        if (!json_obj.contains("Cases") || !json_obj["Cases"].isArray()) {
            cout << "Invalid JSON log. Aborting." << endl;
            exit(1);
        }
        json_file.close();

        // Deleting file contents in preparation to rewrite
        json_file.open(QFile::ReadWrite | QIODevice::Truncate);

        // Add new case to JSON document
        QJsonArray case_array = json_obj["Cases"].toArray();
        case_array.append(new_entry);
        json_obj["Cases"] = case_array;

        // Write the updated log
        QJsonDocument json_doc = QJsonDocument(json_obj);
        json_file.write(json_doc.toJson(QJsonDocument::Indented));
        json_file.close();
    }
    return;
}
