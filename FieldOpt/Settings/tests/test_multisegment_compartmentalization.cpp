/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Settings/tests/test_resource_schedule_segmentation_settings.hpp"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace {

class MultisegmentCompartmentalizationTest : public ::testing::Test {
 protected:
  MultisegmentCompartmentalizationTest() {
      auto partial_deck = TestResources::TestResourceSettings::multisegment_model_settings;
      paths_.SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::norne_deck_);
      paths_.SetPath(Paths::SIM_SCH_FILE, TestResources::ExampleFilePaths::norne_sch_);
      sim_json_ = partial_deck["Simulator"].toObject();
      mod_json_ = partial_deck["Model"].toObject();
  }
  virtual ~MultisegmentCompartmentalizationTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}

  QJsonObject sim_json_;
  QJsonObject mod_json_;
  Paths paths_;
};

TEST_F(MultisegmentCompartmentalizationTest, Constructors) {
    auto sim_settings = Settings::Simulator(sim_json_, paths_);
    auto mod_settings = Settings::Model(mod_json_, paths_);
    for (auto well : mod_settings.wells()) {
        if (well.name == "D-2H") {
            ASSERT_TRUE(well.use_segmented_model);
            ASSERT_TRUE(well.seg_compartment_params.variable_strength);
            ASSERT_FALSE(well.seg_compartment_params.variable_placement);
            ASSERT_EQ(3, well.seg_n_compartments);
            ASSERT_EQ(Settings::Model::WellCompletionType::ICV, well.seg_compartment_params.type);
            ASSERT_FLOAT_EQ(7.85E-5, well.seg_compartment_params.valve_size);
            ASSERT_FLOAT_EQ(1.52E-5, well.seg_tubing.roughness);
            ASSERT_FLOAT_EQ(1.52E-5, well.seg_annulus.roughness);
            ASSERT_FLOAT_EQ(0.1, well.seg_tubing.diameter);
            ASSERT_FLOAT_EQ(8.17E-3, well.seg_annulus.cross_sect_area);
        }
        else {
            ASSERT_FALSE(well.use_segmented_model);
            ASSERT_FALSE(well.seg_compartment_params.variable_strength);
            ASSERT_FALSE(well.seg_compartment_params.variable_placement);
            ASSERT_EQ(0, well.seg_n_compartments);
        }
    }
}

}
