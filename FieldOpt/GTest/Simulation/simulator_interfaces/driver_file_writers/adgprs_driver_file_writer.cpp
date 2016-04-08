#include "GTest/Simulation/results/test_fixture_adgprs.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"
#include "Simulation/simulator_interfaces/simulator.h"
#include "Simulation/simulator_interfaces/driver_file_writers/adgprsdriverfilewriter.h"

namespace {

class AdgprsDriverFileWriterTest : public AdgprsTestFixture {
protected:
    AdgprsDriverFileWriterTest() {
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_, model_);
    }
    virtual ~AdgprsDriverFileWriterTest() {}
    virtual void SetUp() {}
    Simulation::SimulatorInterfaces::AdgprsSimulator *simulator_;
};

TEST_F(AdgprsDriverFileWriterTest, Initialization) {
}

}


