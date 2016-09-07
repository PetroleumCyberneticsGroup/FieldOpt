#include "simulator.h"
#include "simulator_exceptions.h"

namespace Simulation {
    namespace SimulatorInterfaces {

        Simulator::Simulator(Settings::Settings *settings) {
            settings_ = settings;

            if (settings->driver_path().length() == 0)
                throw DriverFileInvalidException("A path to a valid simulator driver file must be specified in the FieldOpt driver file or as a command line parameter.");

            if (!Utilities::FileHandling::FileExists(settings->simulator()->driver_file_path()))
                DriverFileDoesNotExistException(settings->simulator()->driver_file_path());
            initial_driver_file_path_ = settings->simulator()->driver_file_path();
            control_times_ = settings->model()->control_times();

            QStringList tmp = initial_driver_file_path_.split("/");
            initial_driver_file_name_ = tmp.last();

            if (!Utilities::FileHandling::DirectoryExists(settings->output_directory()))
                OutputDirectoryDoesNotExistException(settings->output_directory());
            output_directory_ = settings->output_directory();

            if (settings->build_path().length() > 0)
                build_dir_ = settings->build_path() + "/";

            if (settings->simulator()->custom_simulator_execution_script_path().length() > 0)
                script_path_ = settings->simulator()->custom_simulator_execution_script_path();
            else
                script_path_ = build_dir_ + ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
            script_args_ = (QStringList() << output_directory_ << output_directory_+"/"+initial_driver_file_name_);
        }

        void Simulator::SetOutputDirectory(QString output_directory)
        {
            if (Utilities::FileHandling::DirectoryExists(output_directory)) {
                output_directory_ = output_directory;
                UpdateFilePaths();
            }
            else throw OutputDirectoryDoesNotExistException(output_directory);
        }

        ::Simulation::Results::Results *Simulator::results()
        {
            return results_;
        }

        void Simulator::SetVerbosityLevel(int level) {
            verbosity_level_ = level;
        }

    }
}
