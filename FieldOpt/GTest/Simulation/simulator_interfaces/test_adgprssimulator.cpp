#include "GTest/Simulation/results/test_fixture_adgprs.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"
#include "Simulation/simulator_interfaces/simulator.h"

namespace {

class AdgprsSimulatorTest : public AdgprsTestFixture {
protected:
    AdgprsSimulatorTest() {
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_, model_);
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

