#ifndef TEST_FIXTURE_ADGPRS_H
#define TEST_FIXTURE_ADGPRS_H

#include <gtest/gtest.h>
#include <QString>
#include "Utilities/file_handling/filehandling.h"
#include "GTest/Model/test_fixture_model_base.h"

class AdgprsTestFixture : public ModelBaseTest {
protected:
    AdgprsTestFixture(){}

    QString json_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT.json";
    QString hdf5_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT.SIM.H5";
    QString base_summary_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/5SPOT";
};

#endif // TEST_FIXTURE_ADGPRS_H
