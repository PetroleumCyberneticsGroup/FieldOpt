#include "adgprsdriverfilewriter.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/welspecs.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/compdat.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/wellstre.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/adgprs_driver_parts/adgprs_wellcontrols.h"
#include <iostream>
#include "Utilities/file_handling/filehandling.h"

namespace Simulation { namespace SimulatorInterfaces { namespace DriverFileWriters {

AdgprsDriverFileWriter::AdgprsDriverFileWriter(Utilities::Settings::Settings *settings, Model::Model *model)
{
    model_ = model;
    settings_ = settings;
}

void AdgprsDriverFileWriter::WriteDriverFile(QString output_dir)
{
    auto welspecs = DriverParts::ECLDriverParts::Welspecs(model_->wells());
    auto compdat = DriverParts::ECLDriverParts::Compdat(model_->wells());
    auto wellstre = DriverParts::AdgprsDriverParts::Wellstre(model_->wells());
    auto wellcontrols = DriverParts::AdgprsDriverParts::WellControls(model_->wells());

    QString complete_string = welspecs.GetPartString() + compdat.GetPartString() +
            wellstre.GetPartString() + wellcontrols.GetPartString();

    if (!Utilities::FileHandling::FileExists(output_dir+"/include/wells.in"))
        throw std::runtime_error("Unable to find include/wells.in file to write to.");
    Utilities::FileHandling::WriteStringToFile(complete_string, output_dir+"/include/wells.in");
}

}}}
