#include "main_runner.h"
#include "serial_runner.h"
#include "oneoff_runner.h"
#include "synchronous_mpi_runner.h"

namespace Runner {

    MainRunner::MainRunner(RuntimeSettings *rts)
    {
        runtime_settings_ = rts;

        switch (runtime_settings_->runner_type()) {
            case RuntimeSettings::RunnerType::SERIAL:
                runner_ = new SerialRunner(runtime_settings_);
                break;
            case RuntimeSettings::RunnerType::ONEOFF:
                runner_ = new OneOffRunner(runtime_settings_);
                break;
            case RuntimeSettings::RunnerType::MPISYNC:
                runner_ = new MPI::SynchronousMPIRunner(runtime_settings_);
                break;
            default:
                throw std::runtime_error("Runner type not recognized.");
        }
    }

    void MainRunner::Execute()
    {
        if (runtime_settings_->verb_vector()[0] >= 0) // idx:0 -> run (Runner)
            std::cout << "[run]Starting optimization.-" << std::endl;
        runner_->Execute();
    }

}
