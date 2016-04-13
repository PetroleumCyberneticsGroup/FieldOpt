#ifndef ADGPRSSIMULATOR_H
#define ADGPRSSIMULATOR_H
#include "simulator.h"

#include "Model/model.h"
#include "Utilities/settings/settings.h"
#include "driver_file_writers/adgprsdriverfilewriter.h"

namespace Simulation { namespace SimulatorInterfaces {

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
    AdgprsSimulator(Utilities::Settings::Settings *settings, Model::Model *model);

    // Simulator interface
public:
    void Evaluate();
    void CleanUp();

private:
    QString initial_driver_file_parent_dir_path_;
    QString initial_driver_file_name_;
    QString output_h5_summary_file_path_;
    QString script_path_;
    QStringList script_args_;
    Simulation::SimulatorInterfaces::DriverFileWriters::AdgprsDriverFileWriter *driver_file_writer_;
    void copyDriverFiles(); //!< Copy the original driver files.
    void verifyOriginalDriverFileDirectory(); //!< Ensure that all necessary files are present in the original dir.

    // Simulator interface
protected:
    void UpdateFilePaths();
};

}}

#endif // ADGPRSSIMULATOR_H
