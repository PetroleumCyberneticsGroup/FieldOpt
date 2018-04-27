/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Settings/model.h"
#include "Settings/simulator.h"
#include "Settings/tests/test_resource_schedule_import_settings.hpp"

namespace {

class ScheduleImportSettingsTest : public ::testing::Test {
 protected:
  ScheduleImportSettingsTest() {
      auto partial_deck = TestResources::TestResourceSettings::imported_model_settings;
      paths_.SetPath(Paths::SIM_DRIVER_FILE, "../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA");
      paths_.SetPath(Paths::SIM_SCH_FILE, "../examples/ECLIPSE/norne-simplified/INCLUDE/BC0407_HIST01122006.SCH");
      sim_json_ = partial_deck["Simulator"].toObject();
      mod_json_ = partial_deck["Model"].toObject();
  }
  virtual ~ScheduleImportSettingsTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}

  QJsonObject sim_json_;
  QJsonObject mod_json_;
  Paths paths_;
};

TEST_F(ScheduleImportSettingsTest, Constructors) {
    auto sim_settings = Settings::Simulator(sim_json_, paths_);
    auto mod_settings = Settings::Model(mod_json_, paths_);
    EXPECT_TRUE(true);
}

}
