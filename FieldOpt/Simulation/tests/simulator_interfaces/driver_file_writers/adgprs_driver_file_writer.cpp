#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsDriverFileWriterTest : public ::testing::Test, TestResources::TestResourceModel {
protected:
    AdgprsDriverFileWriterTest() {
        settings_full_->paths().SetPath(Paths::BUILD_DIR, "./");
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::gprs_drv_5spot_.toStdString());
        settings_full_->paths().SetPath(Paths::SIM_DRIVER_DIR, GetParentDirectoryPath(settings_full_->paths().GetPath(Paths::SIM_DRIVER_FILE)));
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_full_, model_);
    }
    virtual ~AdgprsDriverFileWriterTest() {}
    virtual void SetUp() {}
    Simulation::SimulatorInterfaces::AdgprsSimulator *simulator_;
};

TEST_F(AdgprsDriverFileWriterTest, Initialization) {
}

}


