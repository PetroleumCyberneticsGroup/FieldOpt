#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_section.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_insets.h"

using namespace ::Simulation::ECLDriverParts;

namespace {

class DriverPartScheduleTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    DriverPartScheduleTest(){
        schedule_ = new Schedule(model_->wells(), settings_model_->control_times(), insets_);
    }
    virtual ~DriverPartScheduleTest(){}

    Schedule *schedule_;
    ScheduleInsets insets_;
};

TEST_F(DriverPartScheduleTest, Constructor) {
//    std::cout << schedule_->GetPartString().toStdString() << std::endl;
}

}
