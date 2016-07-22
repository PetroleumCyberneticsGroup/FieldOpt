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
    auto wellstre = DriverParts::AdgprsDriverParts::Wellstre(model_->wells(), settings_->simulator()->fluid_model());
    auto wellcontrols = DriverParts::AdgprsDriverParts::WellControls(model_->wells(), settings_->model()->control_times());

    if (!Utilities::FileHandling::FileExists(output_dir+"/include/wells.in"))
        throw std::runtime_error("Unable to find include/wells.in file to write to.");
    else Utilities::FileHandling::WriteStringToFile(welspecs.GetPartString(), output_dir+"/include/welspecs.in");

    if (!Utilities::FileHandling::FileExists(output_dir+"/include/compdat.in"))
        throw std::runtime_error("Unable to find include/compdat.in file to write to.");
    else Utilities::FileHandling::WriteStringToFile(compdat.GetPartString(), output_dir+"/include/compdat.in");

    if (!Utilities::FileHandling::FileExists(output_dir+"/include/controls.in"))
        throw std::runtime_error("Unable to find include/controls.in file to write to.");
    else Utilities::FileHandling::WriteStringToFile(wellstre.GetPartString()
                                                    +wellcontrols.GetPartString(), output_dir+"/include/controls.in");

}

QString AdgprsDriverFileWriter::GetCompdatString()
{
    return DriverParts::ECLDriverParts::Compdat(model_->wells()).GetPartString();
}

}}}
