#include <iostream>
#include "eclsimulator.h"
#include "Utilities/execution.hpp"
#include "simulator_exceptions.h"
#include "Simulation/results/eclresults.h"

namespace Simulation {
    namespace SimulatorInterfaces {

        ECLSimulator::ECLSimulator(Settings::Settings *settings, Model::Model *model)
        : Simulator(settings)
        {
            model_ = model;
            driver_file_writer_ = new DriverFileWriters::EclDriverFileWriter(settings, model_);

            script_path_ = ExecutionScripts::GetScriptPath(settings->simulator()->script_name());
            script_args_ = (QStringList() << output_directory_ << driver_file_writer_->output_driver_file_name_);

            results_ = new Results::ECLResults();
            try {
                results()->ReadResults(driver_file_writer_->output_driver_file_name_);
            } catch (...) {} // At this stage we don't really care if the results can be read, we just want to set the path.
        }

        void ECLSimulator::Evaluate()
        {
            driver_file_writer_->WriteDriverFile();
            ::Utilities::Unix::ExecShellScript(script_path_, script_args_);
            results_->ReadResults(driver_file_writer_->output_driver_file_name_);
        }

        void ECLSimulator::CleanUp()
        {
            QStringList file_endings_to_delete{"DBG", "ECLEND", "ECLRUN", "EGRID", "GRID",
                                               "h5", "INIT","INSPEC", "MSG", "PRT",
                                               "RSSPEC", "UNRST"};
            QString base_file_path = driver_file_writer_->output_driver_file_name_.split(".DATA").first();
            for (QString ending : file_endings_to_delete) {
                Utilities::FileHandling::DeleteFile(base_file_path + "." + ending);
            }
        }

        void ECLSimulator::UpdateFilePaths()
        {
            return;
        }

        QString ECLSimulator::GetCompdatString()
        {
            return driver_file_writer_->GetCompdatString();
        }

        bool ECLSimulator::Evaluate(int timeout) {
            int t = timeout;
            if (timeout < 10) t = 10; // Always let simulations run for at least 10 seconds

            driver_file_writer_->WriteDriverFile();
            std::cout << "Starting monitored simulation with timeout " << timeout << std::endl;
            bool success = ::Utilities::Unix::ExecShellScriptTimeout(script_path_, script_args_, t);
            std::cout << "Monitored simulation done." << std::endl;
            if (success) results_->ReadResults(driver_file_writer_->output_driver_file_name_);
            return success;
        }

    }
}
