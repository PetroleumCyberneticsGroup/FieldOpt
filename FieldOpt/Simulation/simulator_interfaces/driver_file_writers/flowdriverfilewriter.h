#ifndef FIELDOPT_FLOWDRIVERFILEWRITER_H
#define FIELDOPT_FLOWDRIVERFILEWRITER_H

#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Model/model.h"

namespace Simulation {
    namespace SimulatorInterfaces {
        class FlowSimulator;
    }
}

namespace Simulation {
    namespace SimulatorInterfaces {
        namespace DriverFileWriters {
            class FlowDriverFileWriter {
                friend class FlowSimulator;
            private:
                friend class ::Simulation::SimulatorInterfaces::FlowSimulator;
                FlowDriverFileWriter(::Utilities::Settings::Settings *settings, Model::Model *model);
                void WriteDriverFile(QString output_dir);

                Model::Model *model_;
                ::Utilities::Settings::Settings *settings_;
                QString output_driver_file_name_; //!< Path to the driver file to be written.
                QString GetCompdatString();
            };
        }
    }
}

#endif // FIELDOPT_FLOWDRIVERFILEWRITER_H
