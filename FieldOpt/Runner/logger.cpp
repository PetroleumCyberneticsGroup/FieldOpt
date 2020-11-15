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
#include <boost/algorithm/string.hpp>


Logger::Logger(Runner::RuntimeSettings *rts,
               QString output_subdir,
               bool write_logs)
{
    write_logs_ = write_logs;
    is_worker_ = output_subdir.length() > 0;
    verbose_ = rts->verbosity_level();
    output_dir_ = QString::fromStdString(rts->paths().GetPath(Paths::OUTPUT_DIR));
    if (output_subdir.length() > 0) {
        output_dir_ = output_dir_ + "/" + output_subdir + "/";
        Utilities::FileHandling::CreateDirectory(output_dir_);
    }
    opt_log_path_ = output_dir_ + "/log_optimization.csv";
    cas_log_path_ = output_dir_ + "/log_cases.csv";
    cas_ext_log_path_ = output_dir_ + "/log_cases_extended.json";
    ext_log_path_ = output_dir_ + "/log_extended.json";
    run_state_path_ = output_dir_ + "/state_runner.txt";
    summary_prerun_path_ = output_dir_ + output_subdir + "/summary_prerun.md";
    summary_postrun_path_ = output_dir_ + output_subdir + "/summary_postrun.md";
    QStringList log_paths = (QStringList() << cas_log_path_ << cas_ext_log_path_ << opt_log_path_ << ext_log_path_ << run_state_path_
                                           << summary_prerun_path_ << summary_postrun_path_);

    // Delete existing logs if --force flag is on
    if (rts->overwrite_existing()) {
        for (auto path : log_paths) {
            if (Utilities::FileHandling::FileExists(path)) {
                cout << "Force flag on. Deleting " << path.toStdString() << endl;
                Utilities::FileHandling::DeleteFile(path);
            }
        }
    }

    if (write_logs_) {
        // Write CSV headers
        if (!is_worker_) {
            if (rts->paths().IsSet(Paths::ENSEMBLE_FILE)) { // Append OFV std. dev. to case log header if ensemble file path is set
                cas_log_header_.append(" ,       OFvSTD");
            }
            Utilities::FileHandling::WriteLineToFile(cas_log_header_, cas_log_path_);
            Utilities::FileHandling::WriteLineToFile(opt_log_header_, opt_log_path_);
        }

        // Create base JSON document
        QJsonObject json_base;
        json_base.insert("Cases", QJsonArray());
        QJsonDocument json_doc = QJsonDocument(json_base);
        QFile json_file(ext_log_path_);
        json_file.open(QFile::WriteOnly);
        json_file.write(json_doc.toJson(QJsonDocument::Indented));
        json_file.close();

        if (!is_worker_) {
            QFile another_json_file(cas_ext_log_path_);
            another_json_file.open(QFile::WriteOnly);
            another_json_file.write(json_doc.toJson(QJsonDocument::Indented));
            another_json_file.close();
        }
    }
}
void Logger::AddEntry(Loggable *obj) {
    switch (obj->GetLogTarget()) {
        case Loggable::LogTarget::LOG_CASE: logCase(obj); logCaseExtended(obj); break;
        case Loggable::LogTarget::LOG_OPTIMIZER: logOptimizer(obj); break;
        case Loggable::LogTarget::LOG_EXTENDED: logExtended(obj); break;
        case Loggable::LogTarget::LOG_SUMMARY: logSummary(obj); break;
        case Loggable::LogTarget::STATE_RUNNER: logRunnerState(obj); break;
    }
}
void Logger::logRunnerState(Loggable *obj) {
    if (!write_logs_ || !is_worker_) // Only workers should do this
        return;
    stringstream st;
    st << obj->GetState()["case-desc"] << "\n\n";
    st << "Model update done?  " << obj->GetState()["mod-update-done"] << "\n";
    st << "Simulation done?    " << obj->GetState()["sim-done"] << "\n\n";
    st << "Last update: " << obj->GetState()["last-update"];
    Utilities::FileHandling::WriteStringToFile(QString::fromStdString(st.str()), run_state_path_);
}
void Logger::logCase(Loggable *obj) {
    if (!write_logs_ || is_worker_)
        return;
    stringstream entry;
    entry << setw(cas_log_col_widths_["TimeSt"]) << timestamp_string() << " ,";
    entry << setw(cas_log_col_widths_["EvalSt"]) << obj->GetState()["EvalSt"] << " ,";
    entry << setw(cas_log_col_widths_["ConsSt"]) << obj->GetState()["ConsSt"] << " ,";
    entry << setw(cas_log_col_widths_["ErrMsg"]) << obj->GetState()["ErrMsg"] << " ,";
    entry << setw(cas_log_col_widths_["SimDur"]) << timespan_string(obj->GetValues()["SimDur"][0]) << " , ";
    entry << setw(cas_log_col_widths_["WicDur"]) << timespan_string(obj->GetValues()["WicDur"][0]) << " , ";
    entry.precision(6);
    entry << setw(cas_log_col_widths_["OFnVal"]) << scientific << obj->GetValues()["OFnVal"][0] << " ,";
    entry << setw(cas_log_col_widths_["CaseId"]) << obj->GetId().toString().toStdString() << " , ";
    entry.precision(0);
    entry << fixed << setfill('0') << setw(cas_log_col_widths_["IterNr"]) << obj->GetValues()["IterNr"][0] << " , ";
    if (obj->GetValues()["MPSO-NumberOfSwarms"].size() > 0) {
        entry.precision(1);
        entry << fixed << setfill(' ') << setw(13) << obj->GetValues()["Swarm's_r_CO2"][0] << " , ";
        int mpso_nr_of_swarms = obj->GetValues()["MPSO-NumberOfSwarms"][0];
        for (int i = 0; i < mpso_nr_of_swarms; ++i) {
            entry.precision(1);
            entry << setw(10) << obj->GetValues()["r_CO2_" + to_string(i)][0] << " , ";
            entry.precision(2);
            entry << setw(15) << obj->GetValues()["ObjFn_value_" + to_string(i)][0] << " , ";
        }
    }
    if (obj->GetValues().count("OFvSTD") > 0) {
        entry << " , " << setw(cas_log_col_widths_["OFnVal"]) << scientific << obj->GetValues()["OFvSTD"][0];
    }
    string str = entry.str();
    Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), cas_log_path_);
    return;
}
void Logger::logCaseExtended(Loggable *obj){
    if (!write_logs_ || is_worker_)
        return;

    QJsonObject new_entry;

    new_entry.insert("UUID", obj->GetId().toString());

    new_entry.insert("IterNr", obj->GetValues()["IterNr"][0]);

    QJsonArray vars;
    for (auto const a : obj->GetValues()) {
        if(a.first.compare(0, 4, "Var#") == 0) {
            QJsonObject var;
            var.insert(QString::fromStdString(a.first), a.second[0]);
            vars.append(var);
        }
    }
    new_entry.insert("Variables", vars);

    if (obj->GetValues()["MPSO-NumberOfSwarms"].size() > 0) {
        new_entry.insert("Swarm's_r_CO2", obj->GetValues()["Swarm's_r_CO2"][0]);
    }

    // Open existing document
    QFile json_file(cas_ext_log_path_);

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
    return;
}
void Logger::logOptimizer(Loggable *obj) {
    if (!write_logs_ || is_worker_) return;
    stringstream entry;
    entry << setw(opt_log_col_widths_["TimeSt"]) << timestamp_string() << " ,";
    entry << setw(opt_log_col_widths_["TimeEl"]) << timespan_string(obj->GetState()["TimeEl"][0]) << " , ";
    entry << setw(opt_log_col_widths_["TimeIt"]) << timespan_string(obj->GetState()["TimeIt"][0]) << " , ";
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
    entry << obj->GetId().toString().toStdString() << " , ";
    if (obj->GetValues()["MPSO-NumberOfSwarms"].size() > 0) {
        int mpso_nr_of_swarms = obj->GetValues()["MPSO-NumberOfSwarms"][0];
        for (int i = 0; i < mpso_nr_of_swarms; ++i) {
            entry << fixed << setfill(' ') << setw(10) << obj->GetState()["r_CO2_" + to_string(i)] << " , ";
            entry << setw(38) << obj->GetState()["BC_id_" + to_string(i)] << " , ";
            entry << setw(6) << obj->GetState()["BC_iteration_" + to_string(i)] << " , ";
            entry << setw(15) << obj->GetState()["BC_ObjFn_value_" + to_string(i)] << " , ";
        }
    }
    string str = entry.str();
    Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), opt_log_path_);
    return;
}
void Logger::logExtended(Loggable *obj) {
    if (!write_logs_) return;
    QJsonObject new_entry;

    // UUID
    new_entry.insert("UUID", obj->GetId().toString());

    QJsonArray realizations;
    for (auto const a : obj->GetValues()) {
        if (a.first.compare(0, 4, "Rea#") == 0) {
            QJsonObject rea;
            rea.insert(QString::fromStdString(a.first), a.second[0]);
            realizations.append(rea);
        }
    }
    if (realizations.count() > 0) {
        new_entry.insert("Realizations", realizations);
    }

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

    // Compdat string
    new_entry.insert("COMPDAT", QString::fromStdString(obj->GetState()["COMPDAT"]));


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
    return;
}

void Logger::collectExtendedLogs() {
    if (!write_logs_ || is_worker_) return;

    QList<QJsonObject> worker_parts_;
    int rank = 1;

    // Read all the parts
    while (true) {
        QString subpath = output_dir_ + "/rank" + QString::number(rank) + "/log_extended.json";
        if (!Utilities::FileHandling::FileExists(subpath)) {
            break;
        }
        else {
            QFile json_file(subpath);
            json_file.open(QFile::ReadWrite);
            QByteArray json_data = json_file.readAll();
            worker_parts_.append(QJsonDocument::fromJson(json_data).object());
            json_file.close();
            rank++;
        }
    }
    if (worker_parts_.size() == 0) // Return if there were no workers (we're running in serial)
        return;

    // Gather all cases in one array
    QJsonArray all_cases;
    for (QJsonObject part : worker_parts_) {
        all_cases.append(part["Cases"].toArray());
    }

    QFile json_file(ext_log_path_);
    // Deleting file contents in preparation to rewrite
    json_file.open(QFile::ReadWrite | QIODevice::Truncate);

    // Add all cases to JSON document
    QJsonObject json_obj;
    json_obj["Cases"] = all_cases;

    // Write the updated log
    QJsonDocument json_doc = QJsonDocument(json_obj);
    json_file.write(json_doc.toJson(QJsonDocument::Indented));
    json_file.close();
    return;
}

void Logger::logSummary(Loggable *obj) {
    if (obj->GetWellDescriptions().size() > 0) {
        sum_wellmap_ = obj->GetWellDescriptions();
        sum_mod_statemap_ = obj->GetState();
        sum_mod_valmap_ = obj->GetValues();
    }
    else if (obj->GetState().count("verbosity") > 0) {
        sum_rts_statemap_ = obj->GetState();
        sum_rts_valmap_ = obj->GetValues();
    }
    else {
        sum_opt_statemap_ = obj->GetState();
        sum_opt_valmap_ = obj->GetValues();
    }
}

void Logger::FinalizePrerunSummary() {
    if (!write_logs_ || is_worker_) return;

    stringstream sum;

    // ==> Header and TOC <==
    sum << "# FieldOpt run: " << timestamp_string() << "\n\n";
    sum << "* [Run-time settings](#run-time-settings)" << "\n";
    sum << "* [Optimizer](#optimizer)" << "\n";
    sum << "* [Base Case](#base-case)" << "\n";
    sum << "\n";

    // ==> Run-time settings <==
    sum << "## Run-time settings" << "\n\n";
    sum << "| Setting                        | Value                |\n";
    sum << "| ------------------------------ | -------------------- |\n";
    for (auto entry : sum_rts_statemap_) {
        if (entry.first.compare(0, 4, "path") != 0)
            sum << "| " << entry.first << setw(33-entry.first.size()) << right
                << " | " << entry.second << setw(23-entry.second.size()) << right << " |\n";
    }
    sum << "\n";

    // ==> Paths <==
    sum << "### Paths" << "\n\n";
    sum << "| Path                       | Value                          |\n";
    sum << "| -------------------------- | ---------------------------------------- |\n";
    for (auto entry : sum_rts_statemap_) {
        if (entry.first.compare(0, 4, "path") == 0) {
            // Remove "path " from the key.
            string pathn = entry.first;
            pathn.erase(pathn.begin(), pathn.begin()+5);

            // Trim away everything up to "FieldOpt" in the path if possible
            string path = entry.second;
            int fo = path.find("FieldOpt");
            if (fo != string::npos){
                path.erase(path.begin(), path.begin() + fo);
            }
            sum << "| " << pathn << setw(29 - pathn.size()) << right
                << " | " << path << setw(43 - path.size()) << right << " |\n";
        }
    }
    sum << "\n";

    // ==> Optimizer <==
    sum << "## Optimizer" << "\n\n";
    sum << "| Setting                   | Value                          |\n";
    sum << "| ------------------------- | ------------------------------ |\n";
    for (auto entry : sum_opt_statemap_) {
        sum << "| " << entry.first << setw(28-entry.first.size()) << right
            << " | " << entry.second << setfill(' ') << setw(33-entry.second.size()) << right << " |\n";
    }
    sum << "\n";

    // ==> Base Case <==
    // --> Well TOC <--
    sum << "## Base Case" << "\n\n";
    appendWellToc(sum_wellmap_, sum);

    // --> Loop over wells <--
    for (auto w : sum_wellmap_) {
        appendWellDescription(w, sum);
    }

    string str = sum.str();
    Utilities::FileHandling::WriteStringToFile(QString::fromStdString(str), summary_prerun_path_);

    sum_mod_valmap_.clear();
    sum_opt_valmap_.clear();
    sum_rts_valmap_.clear();
    sum_mod_statemap_.clear();
    sum_opt_statemap_.clear();
    sum_rts_statemap_.clear();
    sum_wellmap_.clear();
}

void Logger::FinalizePostrunSummary() {
    if (!write_logs_ || is_worker_) return;

    collectExtendedLogs(); // Collect all the extended json logs into one

    stringstream sum;

    sum << "# FieldOpt summary\n\n";

    // ==> TOC <==
    sum << "* [Optimizer](#optimizer)\n";
    sum << "* [Evaluation](#evaluation)\n";
    sum << "* [Best Case](#best-case)\n";
    sum << "\n";

    // ==> Breif summary table <==
    sum << "| Key                  | Value                          |\n";
    sum << "| -------------------- | ------------------------------ |\n";
    for (auto item : sum_opt_statemap_) {
        if (item.first.compare(0, 2, "bc") != 0) { // Filtering out the best case entries
            sum << "| " << item.first << setw(23-item.first.size()) << right
                << " | " << item.second << setw(33-item.second.size()) << right << " |\n";
        }
    }
    sum << "\n";

    // ==> Evaluation summary <==
    sum << "## Evaluation\n\n";
    sum << "| Cases                | Number     |\n";
    sum << "| -------------------- | ---------- |\n";
    for (auto item : sum_opt_valmap_) {
        sum << "| " << item.first << setw(23-item.first.size()) << right
            << " | " << item.second[0] << setw(10) << right << " |\n";
    }
    sum << "\n";

    // ==> Best Case <==
    sum << "## Best Case\n\n";
    sum << "| Property | Value |\n";
    sum << "| ------------------------------ | ---------------------------------------- |\n";
    for (auto item : sum_opt_statemap_) {
        if (item.first.compare(0, 2, "bc") == 0) { // Taking best case entries
            string key = item.first;
            key.erase(key.begin(), key.begin()+3); // Removing the prepended "bc "
            sum << "| " << key << setw(33-key.size()) << right
                << " | " << item.second << setw(43-item.second.size()) << right << " |\n";
        }
    }
    sum << "\n";

    appendWellToc(sum_wellmap_, sum);

    // --> Loop over wells <--
    for (auto w : sum_wellmap_) {
        appendWellDescription(w, sum);
    }

    sum << "### Compdat\n\n";
    sum << "```\n";
    sum << sum_mod_statemap_["compdat"] << "\n";
    sum << "```\n";

    string str = sum.str();
    Utilities::FileHandling::WriteStringToFile(QString::fromStdString(str), summary_postrun_path_);
}

void Logger::appendWellToc(map<string, Loggable::WellDescription> wellmap, stringstream &sum) {
    sum << "| Name                      | Group      | Type       |\n";
    sum << "| ------------------------- | ---------- | ---------- |\n";
    for (auto w : wellmap) {
        sum << "| [" << w.first << "](#" << boost::algorithm::to_lower_copy(w.first) << ")" << setw(28-w.first.size()*2-5) << right
            << " | " << w.second.group << setw(13-w.second.group.size()) << right
            << " | " << w.second.type  << setw(13-w.second.type.size())  << right << " |\n";
    }
    sum << "\n";
}

void Logger::appendWellDescription(pair<string, Loggable::WellDescription> w, stringstream &sum) {
    sum << "### " << w.first << "\n\n";
    sum << "#### General settings" << "\n\n";
    sum << "| Setting              | Value           |\n";
    sum << "| -------------------- | --------------- |\n";
    sum << "| Name                 | " << w.second.name       << setw(18-w.second.name.size())       << right << " |\n";
    sum << "| Group                | " << w.second.group      << setw(18-w.second.group.size())      << right << " |\n";
    sum << "| Type                 | " << w.second.type       << setw(18-w.second.type.size())       << right << " |\n";
    sum << "| Definition type      | " << w.second.def_type   << setw(18-w.second.def_type.size())   << right << " |\n";
    sum << "| Prefered phase       | " << w.second.pref_phase << setw(18-w.second.pref_phase.size()) << right << " |\n";
    sum << "| Wellbore radius      | " << w.second.wellbore_radius << setw(18-w.second.wellbore_radius.size()) << right << " |\n";
    sum << "\n";
    sum << "#### Spline Definition\n\n";
    sum << "| End point  | Coord | Value      |\n";
    sum << "| ---------- | ----- | ---------- |\n";
    sum << "| Heel       | x     | " << w.second.spline.heel_x << setw(10) << right << " |\n";
    sum << "| Heel       | y     | " << w.second.spline.heel_y << setw(10) << right << " |\n";
    sum << "| Heel       | z     | " << w.second.spline.heel_z << setw(10) << right << " |\n";
    sum << "| Toe        | x     | " << w.second.spline.toe_x  << setw(10) << right << " |\n";
    sum << "| Toe        | y     | " << w.second.spline.toe_y  << setw(10) << right << " |\n";
    sum << "| Toe        | z     | " << w.second.spline.toe_z  << setw(10) << right << " |\n";
    sum << "\n";
    sum << "#### Controls\n";
    sum << "| Time step  | State | Control    | Value      |\n";
    sum << "| ---------- | ----- | ---------- | ---------- |\n";
    for (auto c : w.second.controls) {
        sum << "| " << c.time_step << setw(12)                  << right
            << " | " << c.state    << setw(8-c.state.size())    << right
            << " | " << c.control  << setw(13-c.control.size()) << right
            << " | " << c.value    << setw(10)                  << right << " |\n";
    }
    sum << "\n";
}
