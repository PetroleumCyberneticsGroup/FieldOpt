#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsSimulatorTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    AdgprsSimulatorTest() {
        settings_simulator_->set_driver_file_path(TestResources::ExampleFilePaths::gprs_drv_5spot_);
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_full_, model_);
    }
    virtual ~AdgprsSimulatorTest() {}
    virtual void SetUp() {}
    Simulation::SimulatorInterfaces::Simulator *simulator_;
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

