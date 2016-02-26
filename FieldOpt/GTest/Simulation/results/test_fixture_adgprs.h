#ifndef TEST_FIXTURE_ADGPRS_H
#define TEST_FIXTURE_ADGPRS_H

#include <gtest/gtest.h>
#include <QString>
#include "Utilities/file_handling/filehandling.h"
#include "GTest/Model/test_fixture_model_base.h"

class AdgprsTestFixture : public ModelBaseTest {
protected:
    AdgprsTestFixture(){
        settings_ = new Utilities::Settings::Settings(driver_file_path_, output_directory_);
        settings_->simulator()->set_driver_file_path(sim_driver_path_);
    }
    QString sim_driver_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT.gprs";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output/adgprs";
    QString json_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT.json";
    QString hdf5_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT.SIM.H5";
    QString base_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT";
};

#endif // TEST_FIXTURE_ADGPRS_H
