#include <iostream>
#include "adgprssimulator.h"
#include "Utilities/unix/execution.h"
#include "simulator_exceptions.h"
#include "Simulation/results/adgprsresults.h"

namespace Simulation {
    namespace SimulatorInterfaces {

        AdgprsSimulator::AdgprsSimulator(Utilities::Settings::Settings *settings, Model::Model *model)
        {
            if (!Utilities::FileHandling::FileExists(settings->simulator()->driver_file_path()))
                DriverFileDoesNotExistException(settings->simulator()->driver_file_path());
            initial_driver_file_path_ = settings->simulator()->driver_file_path();

            if (!Utilities::FileHandling::DirectoryExists(settings->output_directory()))
                OutputDirectoryDoesNotExistException(settings->output_directory());
            output_directory_ = settings->output_directory();

            QStringList tmp = initial_driver_file_path_.split("/");
            initial_driver_file_name_ = tmp.last();
            tmp.removeLast();
            initial_driver_file_parent_dir_path_ = tmp.join("/");
            verifyOriginalDriverFileDirectory();
            output_h5_summary_file_path_ = output_directory_ + "/" + initial_driver_file_name_.split(".").first() + ".SIM.H5";

            model_ = model;
            settings_ = settings;
            results_ = new Simulation::Results::AdgprsResults();
            driver_file_writer_ = new DriverFileWriters::AdgprsDriverFileWriter(settings_, model_);

            if (settings->simulator()->custom_simulator_execution_script_path().length() > 0)
                script_path_ = settings->simulator()->custom_simulator_execution_script_path();
            else
                script_path_ = ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
            script_args_ = (QStringList() << output_directory_ << output_directory_+"/"+initial_driver_file_name_);
        }

        void AdgprsSimulator::Evaluate()
        {
            if (results_->isAvailable()) results()->DumpResults();
            copyDriverFiles();
            driver_file_writer_->WriteDriverFile(output_directory_);
            ::Utilities::Unix::ExecShellScript(script_path_, script_args_);

            build_dir_ = settings_->simulator()->fieldopt_build_path();
            if (settings_->simulator()->fieldopt_build_path().length() > 0)
                results_->ReadResults(output_h5_summary_file_path_, build_dir_);
            else
                results_->ReadResults(output_h5_summary_file_path_);

        }

        void AdgprsSimulator::CleanUp()
        {
            Utilities::FileHandling::DeleteFile(output_h5_summary_file_path_);
        }

        void AdgprsSimulator::copyDriverFiles()
        {
            Utilities::FileHandling::CopyFile(initial_driver_file_path_, output_directory_+"/"+initial_driver_file_name_);
            Utilities::FileHandling::CreateDirectory(output_directory_+"/include");
            Utilities::FileHandling::CopyDirectory(initial_driver_file_parent_dir_path_+"/include", output_directory_+"/include");
        }

        void AdgprsSimulator::verifyOriginalDriverFileDirectory()
        {
            QStringList critical_files = {initial_driver_file_path_,
                                          initial_driver_file_parent_dir_path_ + "/include/compdat.in",
                                          initial_driver_file_parent_dir_path_ + "/include/controls.in",
                                          initial_driver_file_parent_dir_path_ + "/include/wells.in",
                                          initial_driver_file_parent_dir_path_ + "/include/welspecs.in"};
            for (auto file : critical_files) {
                if (!Utilities::FileHandling::FileExists(file))
                    throw DriverFileDoesNotExistException(file);
            }
        }

        void AdgprsSimulator::UpdateFilePaths()
        {
            output_h5_summary_file_path_ = output_directory_ + "/" + initial_driver_file_name_.split(".").first() + ".SIM.H5";
            script_args_ = (QStringList() << output_directory_ << output_directory_+"/"+initial_driver_file_name_);
        }

        QString AdgprsSimulator::GetCompdatString()
        {
            return driver_file_writer_->GetCompdatString();
        }

        bool AdgprsSimulator::Evaluate(int timeout) {
            int t = timeout;
            if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds
            if (results_->isAvailable()) results()->DumpResults();
            copyDriverFiles();
            driver_file_writer_->WriteDriverFile(output_directory_);
            std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
            bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_, script_args_, t);
            if (success) {
                results_->ReadResults(output_h5_summary_file_path_);
            }
            return success;
        }

    }
}
