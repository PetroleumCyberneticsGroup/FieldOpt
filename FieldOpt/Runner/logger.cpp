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
#include "logger.h"
#include "Utilities/time.hpp"

namespace Runner {

Logger::Logger(RuntimeSettings *rts,
               QString output_subdir)
{
    verbose_ = rts->verbosity_level();
    output_dir_ = rts->output_dir();
    if (output_subdir.length() > 0)
        output_dir_ = output_dir_ + "/" + output_subdir + "/";
    opt_log_path_ = output_dir_ + "/log_optimization.csv";
    cas_log_path_ = output_dir_ + "/log_cases.csv";
    QStringList log_paths = (QStringList() << cas_log_path_ << opt_log_path_);

    // Delete existing logs if --force flag is on
    if (rts->overwrite_existing()) {
        for (auto path : log_paths) {
            if (Utilities::FileHandling::FileExists(path)) {
                std::cout << "Force flag on. Deleting " << path.toStdString() << std::endl;
                Utilities::FileHandling::DeleteFile(path);
            }
        }
    }

    // Write headers
    Utilities::FileHandling::WriteLineToFile(cas_log_header_,
                                             cas_log_path_);
    Utilities::FileHandling::WriteLineToFile(opt_log_header_, opt_log_path_);
}
void Logger::AddEntry(Loggable &obj) {
    switch (obj.GetLogTarget()) {
        case Loggable::LogTarget::LOG_CASE: logCase(obj);
        case Loggable::LogTarget::LOG_OPTIMIZER: logOptimizer(obj);
        case Loggable::LogTarget::LOG_EXTENDED: logExtended(obj);
    }
}

void Logger::logCase(Loggable &obj) {
    stringstream entry;
    entry << setw(cas_log_col_widths_.at("TimeSt")) << timestamp_string() << " ,";
    entry << setw(cas_log_col_widths_.at("EvalSt")) << obj.GetState()["EvalSt"] << " ,";
    entry << setw(cas_log_col_widths_.at("ConsSt")) << obj.GetState()["ConsSt"] << " ,";
    entry << setw(cas_log_col_widths_.at("ErrMsg")) << obj.GetState()["ErrMsg"] << " ,";
    entry << setw(cas_log_col_widths_.at("SimDur")) << timespan_string(obj.GetValues()["SimDur"][0]) << " ,";
    entry << setw(cas_log_col_widths_.at("OFnVal")) << std::scientific << obj.GetValues()["EvalSt"][0] << " ,";
    entry << setw(cas_log_col_widths_.at("CaseId")) << obj.GetId().toString().toStdString();
    string str = entry.str();
    Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), cas_log_path_);
    return;
}// .5E
void Logger::logOptimizer(Loggable &obj) {
//  ,                                 CurBst";
    stringstream entry;
    entry << setw(opt_log_col_widths_.at("TimeSt")) << timestamp_string() << " ,";
    entry << setw(opt_log_col_widths_.at("TimeEl")) << timespan_string(obj.GetState()["TimeEl"][0]) << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("IterNr")) << obj.GetState()["IterNr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("TotlNr")) << obj.GetState()["TotlNr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("EvalNr")) << obj.GetState()["EvalNr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("BkpdNr")) << obj.GetState()["BkpdNr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("TimONr")) << obj.GetState()["TimONr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("FailNr")) << obj.GetState()["FailNr"][0] << " ,";
    entry << std::setfill('0') << setw(opt_log_col_widths_.at("InvlNr")) << obj.GetState()["InvlNr"][0] << " ,";
    entry << setw(opt_log_col_widths_.at("CBOFnV")) << std::scientific << obj.GetValues()["CBOFnV"][0] << " ,";
    entry << setw(opt_log_col_widths_.at("CurBst")) << obj.GetId().toString().toStdString();
    return;
}
void Logger::logExtended(Loggable &obj) {
    /// \todo Implement this
    return;
}

}
