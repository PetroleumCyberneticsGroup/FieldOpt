#ifndef ADGPRSSIMULATOR_H
#define ADGPRSSIMULATOR_H
#include "simulator.h"

#include "Model/model.h"
#include "Settings/settings.h"
#include "driver_file_writers/adgprsdriverfilewriter.h"

namespace Simulation {
    namespace SimulatorInterfaces {

/*!
 * \brief The AdgprsSimulator class implements simulation of models using the AD-GPRS reservoir simulator.
 *
 * This class should not be used directly except for instantiation. All other actions should be
 * called through the Simulator class. The intended use is as follows:
 *
 * \code
 *  Simulator sim = new AdgprsSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 *
 * \todo Support custom execution commands.
 */
        class AdgprsSimulator : public Simulator
        {
        public:
            AdgprsSimulator(Settings::Settings *settings, Model::Model *model);

            // Simulator interface
        public:
            void Evaluate();
            void CleanUp();
            virtual bool Evaluate(int timeout) override;

        private:
            QString initial_driver_file_parent_dir_path_;
            QString output_h5_summary_file_path_;
            Simulation::SimulatorInterfaces::DriverFileWriters::AdgprsDriverFileWriter *driver_file_writer_;
            void copyDriverFiles(); //!< Copy the original driver files.
            void verifyOriginalDriverFileDirectory(); //!< Ensure that all necessary files are present in the original dir.

            // Simulator interface
        protected:
            void UpdateFilePaths();

            // Simulator interface
        public:
            QString GetCompdatString();
        };

    }
}

#endif // ADGPRSSIMULATOR_H
