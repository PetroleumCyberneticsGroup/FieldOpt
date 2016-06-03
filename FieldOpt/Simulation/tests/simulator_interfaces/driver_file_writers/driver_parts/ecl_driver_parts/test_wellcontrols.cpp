#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/wellcontrols.h"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartWellControlsTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartWellControlsTest(){
        well_controls_ = new WellControls(model_->wells());
    }
    virtual ~DriverPartWellControlsTest(){}

    WellControls *well_controls_;
};

TEST_F(DriverPartWellControlsTest, Constructor) {
    //std::cout << well_controls_->GetPartString().toStdString() << std::endl;
}

}
