#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/runspec_section.h"
#include "Utilities/filehandling.hpp"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartRunspecTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartRunspecTest(){
        QStringList *driver_file_contents = Utilities::FileHandling::ReadFileToStringList(settings_simulator_->driver_file_path());
        runspec_ = new Runspec(driver_file_contents, model_->wells());
    }
    virtual ~DriverPartRunspecTest(){}

    Runspec *runspec_;
};

TEST_F(DriverPartRunspecTest, Constructor) {
    //std::cout << runspec_->GetPartString().toStdString() << std::endl;
}

}
