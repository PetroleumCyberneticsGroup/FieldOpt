#include <results/eclresults.h>
#include <Utilities/unix/execution.h>
#include <iostream>
#include "flowsimulator.h"
#include "simulator_exceptions.h"

namespace Simulation {
    namespace SimulatorInterfaces {

        FlowSimulator::FlowSimulator(Utilities::Settings::Settings *settings, Model::Model *model)
                : Simulator(settings)
        {
            model_ = model;
            results_ = new Simulation::Results::ECLResults();
            driver_file_writer_ = new DriverFileWriters::FlowDriverFileWriter(settings_, model_);

            // Setting paths
            QStringList tmp = initial_driver_file_path_.split("/");
            tmp.removeLast();
            initial_driver_file_parent_dir_path_ = tmp.join("/");
            verifyOriginalDriverFileDirectory();
        }

        void FlowSimulator::Evaluate() {
            if (results_->isAvailable()) results_->DumpResults();
            copyDriverFiles();
            driver_file_writer_->WriteDriverFile(output_directory_);
            ::Utilities::Unix::ExecShellScript(script_path_, script_args_);
            results_->ReadResults(driver_file_writer_->output_driver_file_name_);
        }

        void FlowSimulator::verifyOriginalDriverFileDirectory() {
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

        void FlowSimulator::copyDriverFiles() {
            Utilities::FileHandling::CopyFile(initial_driver_file_path_, output_directory_+"/"+initial_driver_file_name_);
            Utilities::FileHandling::CreateDirectory(output_directory_+"/include");
            Utilities::FileHandling::CopyDirectory(initial_driver_file_parent_dir_path_+"/include", output_directory_+"/include");
        }

        QString FlowSimulator::GetCompdatString() {
            return driver_file_writer_->GetCompdatString();
        }

        bool FlowSimulator::Evaluate(int timeout) {
            int t = timeout;
            if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds
            if (results_->isAvailable()) results()->DumpResults();
            copyDriverFiles();
            driver_file_writer_->WriteDriverFile(output_directory_);
            std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
            bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_, script_args_, t);
            if (success) {
                results_->ReadResults(driver_file_writer_->output_driver_file_name_);
            }
            return success;
        }
    }
}
