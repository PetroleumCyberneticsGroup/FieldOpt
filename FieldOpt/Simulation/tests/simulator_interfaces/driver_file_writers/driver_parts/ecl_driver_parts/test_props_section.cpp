#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/props_section.h"
#include "Utilities/filehandling.hpp"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartPropsTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartPropsTest(){
        QStringList *driver_file_contents = Utilities::FileHandling::ReadFileToStringList(settings_simulator_->driver_file_path());
        props_ = new Props(driver_file_contents);
    }
    virtual ~DriverPartPropsTest(){}

    Props *props_;
};

TEST_F(DriverPartPropsTest, Constructor) {
    //std::cout << props_->GetPartString().toStdString() << std::endl;
}

}
