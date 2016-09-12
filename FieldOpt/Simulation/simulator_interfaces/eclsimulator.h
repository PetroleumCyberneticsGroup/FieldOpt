#ifndef ECLSIMULATOR_H
#define ECLSIMULATOR_H

#include "simulator.h"
#include "driver_file_writers/ecldriverfilewriter.h"
#include "Model/model.h"
#include <QStringList>

namespace Simulation {
    namespace SimulatorInterfaces {


/*!
 * \brief The ECLSimulator class implements simulation of models using the ECLIPSE reservoir simulator.
 *
 * This class should not be used directly except for instantiation. All other actions should be
 * called through the Simulator class. The intended use is as follows:
 *
 * \code
 *  Simulator sim = new ECLSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 *
 * \todo Support custom execution commands.
 */
        class ECLSimulator : public Simulator
        {
        public:
            ECLSimulator(Settings::Settings *settings, Model::Model *model);

            /*!
             * \brief Evaluate Executes the simulation of the current model. The evaluation is blocking.
             */
            void Evaluate();

            /*!
             * \brief CleanUp Deletes files created during the simulation.
             * All files except the .DATA, .UNSMRY, .SMSPEC  and .LOG are deleted.
             */
            void CleanUp();

        private:
            DriverFileWriters::EclDriverFileWriter *driver_file_writer_;
            QString script_path_;
            QStringList script_args_;

            // Simulator interface
        protected:
            void UpdateFilePaths();

            // Simulator interface
        public:
            QString GetCompdatString();

            virtual bool Evaluate(int timeout) override;
        };

    }
}
#endif // ECLSIMULATOR_H
