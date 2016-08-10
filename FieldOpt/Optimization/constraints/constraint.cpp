#include "constraint.h"

namespace Optimization {
    namespace Constraints {

        Constraint::Constraint() {
            logging_enabled_ = false;
        }

        void Constraint::EnableLogging(QString output_directory_path) {
            logging_enabled_ = true;
            constraint_log_path_ = output_directory_path + "/log_constraints.txt";
        }

        void Constraint::SetVerbosityLevel(int level) {
            verbosity_level_ = level;
        }


    }
}

