#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/solution_section.h"
#include "Utilities/file_handling/filehandling.hpp"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartSolutionTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartSolutionTest(){
        QStringList *driver_file_contents = Utilities::FileHandling::ReadFileToStringList(settings_simulator_->driver_file_path());
        solution_ = new Solution(driver_file_contents);
    }
    virtual ~DriverPartSolutionTest(){}

    Solution *solution_;
};

TEST_F(DriverPartSolutionTest, Constructor) {
    //std::cout << solution_->GetPartString().toStdString() << std::endl;
}

}
