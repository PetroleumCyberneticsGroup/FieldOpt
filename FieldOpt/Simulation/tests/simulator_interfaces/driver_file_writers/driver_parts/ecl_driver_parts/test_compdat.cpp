#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/compdat.h"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartCompdatTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartCompdatTest(){
        compdat_ = new Compdat(model_->wells());
    }
    virtual ~DriverPartCompdatTest(){}

    Compdat *compdat_;
};

TEST_F(DriverPartCompdatTest, Constructor) {
    //std::cout << compdat_->GetPartString().toStdString() << std::endl;
}

}

