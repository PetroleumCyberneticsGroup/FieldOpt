#ifndef SIMULATOR
#define SIMULATOR

#include <QString>
#include "Model/model.h"
#include "Simulation/results/results.h"
#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Simulation/execution_scripts/execution_scripts.h"

namespace Simulation {
    namespace SimulatorInterfaces {

/*!
 * \brief The Simulator class acts as an interface for all reservoir simulators.
 *
 * The constructor for this class is not intended to be used directly. The intended use
 * is as follows:
 *
 * \code
 *  Simulator sim = new ECLSimulator();
 *  sim.SetOutputDirectory("some/path");
 *  sim.Evaluate();
 *  sim.CleanUp();
 * \endcode
 */
        class Simulator {
        public:
            /*!
             * \brief SetOutputDirectory Set the directory in which to execute the simulation.
             */
            void SetOutputDirectory(QString output_directory);

            /*!
             * \brief results Get the simulation results.
             */
            ::Simulation::Results::Results *results();

            /*!
             * \brief Evaluate Writes the driver file and executes a simulation of the model.
             */
            virtual void Evaluate() = 0;

            /*!
             * \brief Evaluate Writes the driver file and executes a simulation of the model. The simulation
             * is terminated after the amount of seconds provided in the timeout argument.
             * @param timeout Number of seconds before the simulation should be terminated.
             * @return True if the simuation completes before the set timeout, otherwise false.
             */
            virtual bool Evaluate(int timeout) = 0;

            /*!
             * \brief CleanUp Perform cleanup after simulation, i.e. delete output files.
             */
            virtual void CleanUp() = 0;

            /*!
             * \brief GetCompdatString Get the compdat section used in the simulation's driver file.
             * \return String containing the compdat section.
             */
            virtual QString GetCompdatString() = 0;

            void SetVerbosityLevel(int level);

        protected:
            /*!
             * Set various path variables. Should only be called by child classes.
             * @param settings
             * @return
             */
            Simulator(Settings::Settings *settings);

            QString initial_driver_file_path_; //!< Path to the driver file to be used as a base for the generated driver files.
            QString output_directory_; //!< The directory in which to write new driver files and execute simulations.
            QString initial_driver_file_name_;

            ::Simulation::Results::Results *results_;
            Settings::Settings *settings_;
            Model::Model *model_;
            QString build_dir_;
            QString script_path_;
            QStringList script_args_;
            QList<int> control_times_;
            virtual void UpdateFilePaths() = 0;
            int verbosity_level_; //!< Verbosity level for runtime console logging.
        };

    }
}

#endif // SIMULATOR

