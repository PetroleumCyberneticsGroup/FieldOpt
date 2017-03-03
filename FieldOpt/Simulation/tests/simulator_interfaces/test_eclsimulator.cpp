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
#include "Simulation/simulator_interfaces/eclsimulator.h"

using namespace Simulation::SimulatorInterfaces;
namespace {

class ECLSimulatorTest : public testing::Test, TestResources::TestResourceModel {
protected:
    ECLSimulatorTest() {
        simulator_ = new ECLSimulator(settings_full_, model_);
    }
    Simulator *simulator_;
};

TEST_F(ECLSimulatorTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(ECLSimulatorTest, Evaluate) {
    //simulator_->Evaluate();
}

TEST_F(ECLSimulatorTest, CleanUp) {
    //simulator_->CleanUp();
}

}
