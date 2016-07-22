#ifndef FIELDOPT_FLOWSIMULATOR_H
#define FIELDOPT_FLOWSIMULATOR_H

#include "simulator.h"
#include "driver_file_writers/flowdriverfilewriter.h"
#include "Model/model.h"

namespace Simulation {
    namespace SimulatorInterfaces {
        class FlowSimulator : public Simulator {
        public:
            FlowSimulator(Utilities::Settings::Settings *settings, Model::Model *model);

            virtual void Evaluate() override;
            virtual bool Evaluate(int timeout) override;
            virtual void CleanUp() override;
            virtual void UpdateFilePaths() override;
            QString GetCompdatString();

        private:
            DriverFileWriters::FlowDriverFileWriter *driver_file_writer_;
            QString script_path_;
            QStringList script_args_;
            QString initial_driver_file_parent_dir_path_;

            void verifyOriginalDriverFileDirectory();
            void copyDriverFiles();
        };
    }
}


#endif //FIELDOPT_FLOWSIMULATOR_H
