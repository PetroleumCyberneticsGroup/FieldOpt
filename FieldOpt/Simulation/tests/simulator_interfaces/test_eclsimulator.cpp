#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/eclsimulator.h"

using namespace Simulation::SimulatorInterfaces;
namespace {

class ECLSimulatorTest : public testing::Test, TestResources::TestResourceModel {
protected:
    ECLSimulatorTest() {
        simulator_ = new ECLSimulator(settings_full_, model_);
    }
    Simulator *simulator_;
};

TEST_F(ECLSimulatorTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(ECLSimulatorTest, Evaluate) {
    //simulator_->Evaluate();
}

TEST_F(ECLSimulatorTest, CleanUp) {
    //simulator_->CleanUp();
}

}
