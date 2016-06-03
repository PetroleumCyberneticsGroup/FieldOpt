#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/welspecs.h"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartWelspecsTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartWelspecsTest(){
        welspecs_ = new Welspecs(model_->wells());
    }
    virtual ~DriverPartWelspecsTest(){}

    Welspecs *welspecs_;
};

TEST_F(DriverPartWelspecsTest, Constructor) {
    //std::cout << welspecs_->GetPartString().toStdString() << std::endl;
}

}
