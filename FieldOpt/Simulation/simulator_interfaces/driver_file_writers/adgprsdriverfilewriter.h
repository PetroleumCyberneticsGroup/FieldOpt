#ifndef ADGPRSDRIVERFILEWRITER_H
#define ADGPRSDRIVERFILEWRITER_H

#include "Settings/settings.h"
#include "Settings/simulator.h"
#include "Model/model.h"

namespace Simulation { namespace SimulatorInterfaces {
    class AdgprsSimulator;
}}

namespace Simulation { namespace SimulatorInterfaces { namespace DriverFileWriters {

/*!
 * \brief The AdgprsDriverFileWriter class is responsible for writing AD-GPRS driver files representing
 * the model. It uses many of the parts created for the ECL simulator, as many of the keywords are
 * more or less identical.
 */
class AdgprsDriverFileWriter
{
private:
    friend class ::Simulation::SimulatorInterfaces::AdgprsSimulator;
    AdgprsDriverFileWriter(::Settings::Settings *settings, Model::Model *model);
    void WriteDriverFile(QString output_dir);

    Model::Model *model_;
    Settings::Settings *settings_;
};

}}}

#endif // ADGPRSDRIVERFILEWRITER_H
