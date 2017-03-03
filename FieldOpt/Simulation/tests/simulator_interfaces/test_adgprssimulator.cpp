/******************************************************************************
   Copyright (C) 2016-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/adgprssimulator.h"

namespace {

class AdgprsSimulatorTest : public ::testing::Test, public TestResources::TestResourceModel {
protected:
    AdgprsSimulatorTest() {
        settings_simulator_->set_driver_file_path(TestResources::ExampleFilePaths::gprs_drv_5spot_);
        simulator_ = new Simulation::SimulatorInterfaces::AdgprsSimulator(settings_full_, model_);
    }
    virtual ~AdgprsSimulatorTest() {}
    virtual void SetUp() {}
    Simulation::SimulatorInterfaces::Simulator *simulator_;
};

TEST_F(AdgprsSimulatorTest, ReadFile) {
    EXPECT_TRUE(true);
}

TEST_F(AdgprsSimulatorTest, Evaluate) {
//    simulator_->Evaluate();
    EXPECT_TRUE(true);
}

//TEST_F(AdgprsSimulatorTest, CleanUp) {
//    EXPECT_TRUE(false);
//}

}

