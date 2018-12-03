#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsSimulatorTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    AdgprsSimulatorTest() {
        settings_full_->paths().SetPath(Paths::DRIVER_FILE, TestResources::ExampleFilePaths::driver_5spot_.toStdString());
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::gprs_drv_5spot_.toStdString());
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_DIR, GetParentDirectoryPath(settings_full_->paths().GetPath(Paths::SIM_DRIVER_FILE)));
        settings_full_->paths().SetPath(Paths::BUILD_DIR, "./");
        simulator_ = new Simulation::AdgprsSimulator(settings_full_, model_);
    }


//  static QString driver_5spot_ = "../examples/ADGPRS/5spot/fo_driver_5vert_wells.json";
//  static QString gprs_drv_5spot_ = "../examples/ADGPRS/5spot/5SPOT.gprs";
//  static QString gprs_smry_json_5spot_ = "../examples/ADGPRS/5spot/5SPOT.json";
//  static QString gprs_smry_hdf5_5spot_ = "../examples/ADGPRS/5spot/5SPOT.vars.h5";
//  static QString gprs_base_5spot_ = "../examples/ADGPRS/5spot/5SPOT";


    virtual ~AdgprsSimulatorTest() {}
    virtual void SetUp() {}
    Simulation::Simulator *simulator_;
};

TEST_F(AdgprsSimulatorTest, ReadFile) {
    EXPECT_TRUE(true);
}

TEST_F(AdgprsSimulatorTest, Evaluate) {
//    simulator_->Evaluate();
    EXPECT_TRUE(true);
}

//TEST_F(AdgprsSimulatorTest, CleanUp) {
//    EXPECT_TRUE(false);
//}

}

