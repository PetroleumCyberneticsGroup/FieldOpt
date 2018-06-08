//
// Created by einar on 6/6/18.
//

#include <gtest/gtest.h>
#include <wells/segmented_well.h>
#include "Settings/settings.h"
#include "Settings/tests/test_resource_schedule_segmentation_settings.hpp"
#include "Settings/paths.h"

namespace {

class SegmentedWellTest : public ::testing::Test {
 protected:
  SegmentedWellTest() {
      auto partial_deck = TestResources::TestResourceSettings::multisegment_model_settings;
      paths_.SetPath(Paths::SIM_DRIVER_FILE, "../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA");
      paths_.SetPath(Paths::GRID_FILE, "../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.EGRID");
      paths_.SetPath(Paths::SIM_SCH_FILE, "../examples/ECLIPSE/norne-simplified/INCLUDE/BC0407_HIST01122006.SCH");
      sim_json_ = partial_deck["Simulator"].toObject();
      mod_json_ = partial_deck["Model"].toObject();
      sim_settings_ = new Settings::Simulator(sim_json_, paths_);
      mod_settings_ = new Settings::Model(mod_json_, paths_);
      varcont_ = new Model::Properties::VariablePropertyContainer();
      grid_ = new Reservoir::Grid::ECLGrid(paths_.GetPath(Paths::GRID_FILE));
      for (int i = 0; i < mod_settings_->wells().size(); ++i) {
          if (QString::compare(mod_settings_->wells()[i].name, "D-2H") == 0) {
              d_2h_idx_ = i;
              break;
          }
      }
  }

  QJsonObject sim_json_;
  QJsonObject mod_json_;
  Settings::Simulator *sim_settings_;
  Settings::Model *mod_settings_;
  Model::Properties::VariablePropertyContainer *varcont_;
  Reservoir::Grid::Grid *grid_;
  Paths paths_;
  int d_2h_idx_;
  ::Model::Wells::SegmentedWell *d_2h_;
};

TEST_F(SegmentedWellTest, Constructor ) {
    // Verifying settings
    EXPECT_TRUE(mod_settings_->wells()[d_2h_idx_].use_segmented_model);
    EXPECT_EQ(3, mod_settings_->wells()[d_2h_idx_].seg_n_compartments);

    // Construct well
    d_2h_ = new Model::Wells::SegmentedWell(*mod_settings_, d_2h_idx_, varcont_, grid_);
    double length = d_2h_->trajectory()->GetLength();
}

TEST_F(SegmentedWellTest, Compartments ) {
    d_2h_ = new Model::Wells::SegmentedWell(*mod_settings_, d_2h_idx_, varcont_, grid_);
    double length = d_2h_->trajectory()->GetLength();

    EXPECT_EQ(3, d_2h_->GetCompartments().size());
    EXPECT_EQ(4, d_2h_->GetPackers().size());
    EXPECT_EQ(3, d_2h_->GetICDs().size());

    EXPECT_FLOAT_EQ(0.0, d_2h_->GetCompartments()[0].start_packer->md());

    // some error is allowed here, because the packers are snapped to
    // spline-cell intersections.
    EXPECT_NEAR(894.0, d_2h_->GetCompartments()[2].end_packer->md(), 6);

    EXPECT_TRUE(d_2h_->GetCompartments()[0].end_packer == d_2h_->GetCompartments()[1].start_packer);
    EXPECT_NEAR(length/3.0, d_2h_->GetCompartments()[0].end_packer->md(), 30);
    EXPECT_NEAR(length/3.0, d_2h_->GetCompartments()[1].start_packer->md(), 30);
    EXPECT_NEAR(2*length/3.0, d_2h_->GetCompartments()[1].end_packer->md(), 30);
    EXPECT_NEAR(2*length/3.0, d_2h_->GetCompartments()[2].start_packer->md(), 30);

    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[0].icd->valveSize());
    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[1].icd->valveSize());
    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[2].icd->valveSize());
    EXPECT_NEAR(0.0,          d_2h_->GetCompartments()[0].icd->md(), 30);
    EXPECT_NEAR(length/3.0,   d_2h_->GetCompartments()[1].icd->md(), 30);
    EXPECT_NEAR(2*length/3.0, d_2h_->GetCompartments()[2].icd->md(), 30);
}

}
