/******************************************************************************
   Created by einar on 4/11/18.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <QString>

#include "Settings/tests/test_resource_example_file_paths.hpp"
#include "Settings/deck_parser.h"

using namespace Settings;
using namespace TestResources::ExampleFilePaths;

namespace {

class DeckParserTest : public ::testing::Test {
 protected:
  DeckParserTest() { }
  virtual ~DeckParserTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(DeckParserTest, Initialization) {
    DeckParser dp = DeckParser("/home/einar/Documents/GitHub/PCG/Models/ECLIPSE/olympus_ensemble/OLYMPUS_1/OLYMPUS_1.DATA");
//    DeckParser dp = DeckParser(TestResources::ExampleFilePaths::deck_horzwel_.toStdString());
    auto wells = dp.GetWellData();

//    for (int i = 0; i < num_timesteps_; ++i) {
//        std::cout << i << "\t" << dp.GetTimeDays()[i] << "\t" << dp.GetTimeDates()[i] << std::endl;
//    }
//
//    std::cout << wells[0].toString();
//
//    for (auto control : wells[0].controls) {
//        std::cout << control.toString() << std::endl;
//    }
    auto events = dp.GetEvents();
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < dp.GetTimeDays().size(); ++i) {
        if (events->hasEvent(Opm::ScheduleEvents::COMPLETION_CHANGE, i)) {
            std::cout << "New well event at timestep " << i << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


}
