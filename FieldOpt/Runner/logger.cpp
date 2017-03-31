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

}
