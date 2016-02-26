#include "adgprsdriverfilewriter.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/welspecs.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/compdat.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/wellstre.h"

namespace Simulation { namespace SimulatorInterfaces { namespace DriverFileWriters {

AdgprsDriverFileWriter::AdgprsDriverFileWriter(Utilities::Settings::Settings *settings, Model::Model *model)
{
    model_ = model;
    settings_ = settings;
}

void AdgprsDriverFileWriter::WriteDriverFile()
{
    auto welspecs = DriverParts::ECLDriverParts::Welspecs(model_->wells());
    auto compdat = DriverParts::ECLDriverParts::Compdat(model_->wells());
    auto wellstre = DriverParts::AdgprsDriverParts::Wellstre(model_->wells());
}

}}}
