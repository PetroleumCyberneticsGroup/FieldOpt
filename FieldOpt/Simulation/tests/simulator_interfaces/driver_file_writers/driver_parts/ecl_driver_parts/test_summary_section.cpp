#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/summary_section.h"
#include "Utilities/file_handling/filehandling.h"

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartSummaryTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartSummaryTest(){
        QStringList *driver_file_contents = Utilities::FileHandling::ReadFileToStringList(settings_simulator_->driver_file_path());
        summary_ = new Summary(driver_file_contents);
    }
    virtual ~DriverPartSummaryTest(){}

    Summary *summary_;
};

TEST_F(DriverPartSummaryTest, Constructor) {
    //std::cout << summary_->GetPartString().toStdString() << std::endl;
}

}
