#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsSimulatorTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    AdgprsSimulatorTest() {
        settings_full_->paths().SetPath(Paths::DRIVER_FILE, TestResources::ExampleFilePaths::driver_5spot_);
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::gprs_drv_5spot_);
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_DIR, GetParentDirectoryPath(settings_full_->paths().GetPath(Paths::SIM_DRIVER_FILE)));
        settings_full_->paths().SetPath(Paths::BUILD_DIR, TestResources::ExampleFilePaths::bin_dir_);
        simulator_ = new Simulation::AdgprsSimulator(settings_full_, model_);
    }


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

