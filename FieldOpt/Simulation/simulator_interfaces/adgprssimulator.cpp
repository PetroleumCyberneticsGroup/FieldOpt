#include <iostream>
#include "adgprssimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/adgprsresults.h"

namespace Simulation {
    namespace SimulatorInterfaces {

        AdgprsSimulator::AdgprsSimulator(Settings::Settings *settings, Model::Model *model)
        : Simulator(settings)
        {
            QStringList tmp = initial_driver_file_path_.split("/");
            tmp.removeLast();
            initial_driver_file_parent_dir_path_ = tmp.join("/");
            verifyOriginalDriverFileDirectory();
            output_h5_summary_file_path_ = output_directory_ + "/" + initial_driver_file_name_.split(".").first() + ".vars.h5";

            model_ = model;
            results_ = new Simulation::Results::AdgprsResults();
            driver_file_writer_ = new DriverFileWriters::AdgprsDriverFileWriter(settings_, model_);
        }

        void AdgprsSimulator::Evaluate()
        {
            if (results_->isAvailable()) results()->DumpResults();
            copyDriverFiles();
            driver_file_writer_->WriteDriverFile(output_directory_);
            ::Utilities::Unix::ExecShellScript(script_path_, script_args_);
            results_->ReadResults(output_h5_summary_file_path_);
        }

        void AdgprsSimulator::CleanUp()
        {
            Utilities::FileHandling::DeleteFile(output_h5_summary_file_path_);
        }

        void AdgprsSimulator::copyDriverFiles()
        {
            Utilities::FileHandling::CopyFile(initial_driver_file_path_, output_directory_+"/"+initial_driver_file_name_, true);
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
            output_h5_summary_file_path_ = output_directory_ + "/" + initial_driver_file_name_.split(".").first() + ".vars.h5";
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
