#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsDriverFileWriterTest : public ::testing::Test, TestResources::TestResourceModel {
protected:
    AdgprsDriverFileWriterTest() {
        settings_simulator_->set_driver_file_path(TestResources::ExampleFilePaths::gprs_drv_5spot_);
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_full_, model_);
    }
    virtual ~AdgprsDriverFileWriterTest() {}
    virtual void SetUp() {}
    Simulation::SimulatorInterfaces::AdgprsSimulator *simulator_;
};

TEST_F(AdgprsDriverFileWriterTest, Initialization) {
}

}


