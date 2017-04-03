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

#include "logger.h"
#include "Utilities/time.hpp"

namespace Runner {

Logger::Logger(RuntimeSettings *rts, QString output_subdir)
{
    verbose_ = rts->verbosity_level();
    output_dir_ = rts->output_dir();
    if (output_subdir.length() > 0) output_dir_.append("/" + output_subdir + "/");
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
}

void Logger::AddEntry(Loggable object) {
    return;
}

}
