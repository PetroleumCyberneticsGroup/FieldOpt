#ifndef ADGPRSSIMULATOR_H
#define ADGPRSSIMULATOR_H
#include "simulator.h"

#include "Model/model.h"
#include "Utilities/settings/settings.h"

namespace Simulation { namespace SimulatorInterfaces {

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
    void copyDriverFiles(); //!< Copy the original driver files.
    void verifyOriginalDriverFileDirectory(); //!< Ensure that all necessary files are present in the original dir.
};

}}

#endif // ADGPRSSIMULATOR_H
