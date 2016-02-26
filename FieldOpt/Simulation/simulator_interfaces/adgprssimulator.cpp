#include "adgprssimulator.h"
#include "Utilities/file_handling/filehandling.h"
#include <QStringList>
#include "simulator_exceptions.h"
#include <iostream>

namespace Simulation { namespace SimulatorInterfaces {

AdgprsSimulator::AdgprsSimulator(Utilities::Settings::Settings *settings, Model::Model *model)
{
    if (!Utilities::FileHandling::FileExists(settings->simulator()->driver_file_path()))
        DriverFileDoesNotExistException(settings->simulator()->driver_file_path());
    initial_driver_file_path_ = settings->simulator()->driver_file_path();

    if (!Utilities::FileHandling::DirectoryExists(settings->output_directory()))
        OutputDirectoryDoesNotExistException(settings->output_directory());
    output_directory_ = settings->output_directory();

    QStringList tmp = initial_driver_file_path_.split("/");
    initial_driver_file_name_ = tmp.last();
    tmp.removeLast();
    initial_driver_file_parent_dir_path_ = tmp.join("/");
    verifyOriginalDriverFileDirectory();
}

void AdgprsSimulator::Evaluate()
{
    copyDriverFiles();
}

void AdgprsSimulator::CleanUp()
{
    // Files to delete: *.SIM.H5 etc
}

void AdgprsSimulator::copyDriverFiles()
{
    Utilities::FileHandling::CopyFile(initial_driver_file_path_, output_directory_+"/"+initial_driver_file_name_);
    Utilities::FileHandling::CreateDirectory(output_directory_+"/include");
    Utilities::FileHandling::CopyDirectory(initial_driver_file_parent_dir_path_+"/include", output_directory_+"/include");
}

void AdgprsSimulator::verifyOriginalDriverFileDirectory()
{
    QStringList critical_files = {initial_driver_file_path_,
                                  initial_driver_file_parent_dir_path_ + "/include/compdat.in",
                                  initial_driver_file_parent_dir_path_ + "/include/controls.in",
                                  initial_driver_file_parent_dir_path_ + "/include/wells.in",
                                  initial_driver_file_parent_dir_path_ + "/include/welspecs.in"};
    foreach (auto file, critical_files) {
        if (!Utilities::FileHandling::FileExists(file))
            throw DriverFileDoesNotExistException(file);
    }
}

}}
