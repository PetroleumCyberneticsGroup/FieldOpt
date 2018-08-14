#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/grid_section.h"
#include "Utilities/filehandling.hpp"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartGridTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartGridTest(){
        QStringList *driver_file_contents = Utilities::FileHandling::ReadFileToStringList(
            QString::fromStdString(settings_full_->paths().GetPath(Paths::SIM_DRIVER_FILE))
        );
        grid_ = new Grid(driver_file_contents);
    }
    virtual ~DriverPartGridTest(){}

    Grid *grid_;
};

TEST_F(DriverPartGridTest, Constructor) {
    //std::cout << grid_->GetPartString().toStdString() << std::endl;
}

}
