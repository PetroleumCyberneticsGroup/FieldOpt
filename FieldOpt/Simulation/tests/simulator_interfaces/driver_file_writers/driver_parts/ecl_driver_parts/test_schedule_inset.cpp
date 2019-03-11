/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <Model/tests/test_resource_model.h>
#include <gtest/gtest.h>
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_section.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/schedule_insets.h"

using namespace ::Simulation::ECLDriverParts;

namespace {

class DriverPartScheduleInsetTest : public ::testing::Test, public TestResources::TestResourceModel {
 protected:
  DriverPartScheduleInsetTest(){
      insets_ = ScheduleInsets(TestResources::ExampleFilePaths::schedule_inset_);
      schedule_ = new Schedule(model_->wells(), settings_model_->control_times(), insets_);
  }
  virtual ~DriverPartScheduleInsetTest(){}

  Schedule *schedule_;
  ScheduleInsets insets_;
};

TEST_F(DriverPartScheduleInsetTest, Constructor) {
//    std::cout << schedule_->GetPartString().toStdString() << std::endl;
}

}
