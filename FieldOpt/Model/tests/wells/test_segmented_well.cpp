//
// Created by einar on 6/6/18.
//

#include <gtest/gtest.h>
#include <algorithm>
#include <wells/well.h>
#include "Settings/settings.h"
#include "Settings/tests/test_resource_schedule_segmentation_settings.hpp"
#include "Settings/paths.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

using namespace Model::Wells;

namespace {

class SegmentedWellTest : public ::testing::Test {
 protected:
  SegmentedWellTest() {
      auto partial_deck = TestResources::TestResourceSettings::multisegment_model_settings;
      paths_.SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::norne_deck_);
      paths_.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::norne_grid_);
      paths_.SetPath(Paths::SIM_SCH_FILE, TestResources::ExampleFilePaths::norne_sch_);
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
  ::Model::Wells::Well *d_2h_;
};

TEST_F(SegmentedWellTest, Constructor ) {
    // Verifying settings
    EXPECT_TRUE(mod_settings_->wells()[d_2h_idx_].use_segmented_model);
    EXPECT_EQ(3, mod_settings_->wells()[d_2h_idx_].seg_n_compartments);

    // Construct well
    d_2h_ = new Model::Wells::Well(*mod_settings_, d_2h_idx_, varcont_, grid_, nullptr);
    double length = d_2h_->trajectory()->GetLength();
}

TEST_F(SegmentedWellTest, Compartments ) {
    d_2h_ = new Model::Wells::Well(*mod_settings_, d_2h_idx_, varcont_, grid_, nullptr);
    double length = d_2h_->trajectory()->GetLength();

    EXPECT_EQ(3, d_2h_->GetCompartments().size());
    EXPECT_EQ(4, d_2h_->GetPackers().size());
    EXPECT_EQ(3, d_2h_->GetICDs().size());

    EXPECT_FLOAT_EQ(0.0, d_2h_->GetCompartments()[0].start_packer->md(length));

    // some error is allowed here, because the packers are snapped to
    // spline-cell intersections.
    EXPECT_NEAR(length, d_2h_->GetCompartments()[2].end_packer->md(length), 6);

    EXPECT_TRUE(d_2h_->GetCompartments()[0].end_packer == d_2h_->GetCompartments()[1].start_packer);
    EXPECT_TRUE(d_2h_->GetCompartments()[1].end_packer == d_2h_->GetCompartments()[2].start_packer);
    EXPECT_TRUE(d_2h_->GetCompartments()[0].end_packer->md(length) > d_2h_->GetCompartments()[0].start_packer->md(length));
    EXPECT_TRUE(d_2h_->GetCompartments()[1].end_packer->md(length) > d_2h_->GetCompartments()[1].start_packer->md(length));
    EXPECT_TRUE(d_2h_->GetCompartments()[2].end_packer->md(length) > d_2h_->GetCompartments()[2].start_packer->md(length));

    EXPECT_NEAR(0.0, d_2h_->GetCompartments()[0].start_packer->md(length), 1.0);
    EXPECT_NEAR(length, d_2h_->GetCompartments()[2].end_packer->md(length), 1.0);

    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[0].icd->valveSize());
    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[1].icd->valveSize());
    EXPECT_FLOAT_EQ(7.85E-5, d_2h_->GetCompartments()[2].icd->valveSize());
    EXPECT_NEAR(0.0,          d_2h_->GetCompartments()[0].icd->md(length), 30);
    EXPECT_NEAR(d_2h_->GetCompartments()[2].start_packer->md(length), d_2h_->GetCompartments()[2].icd->md(length), 1);
}

TEST_F(SegmentedWellTest, SegmentTypes) {
    d_2h_ = new Model::Wells::Well(*mod_settings_, d_2h_idx_, varcont_, grid_, nullptr);
    auto segs = d_2h_->GetSegments();
    EXPECT_EQ(Segment::SegType::TUBING_SEGMENT, segs[0].Type()); // Root segment
    for (int i = 1; i < 4; ++i) {
        EXPECT_EQ(Segment::SegType::TUBING_SEGMENT, segs[i].Type());
    }
    for (int i = 4; i < 7; ++i) {
        EXPECT_EQ(Segment::SegType::ICD_SEGMENT, segs[i].Type());
    }
    for (int i = 7; i < segs.size(); ++i) {
        EXPECT_EQ(Segment::SegType::ANNULUS_SEGMENT, segs[i].Type());
    }
}

TEST_F(SegmentedWellTest, SegmentConnections) {
    d_2h_ = new Model::Wells::Well(*mod_settings_, d_2h_idx_, varcont_, grid_, nullptr);
    auto segs = d_2h_->GetSegments();
    auto tub_segs = d_2h_->GetTubingSegments();
    auto icd_segs = d_2h_->GetICDSegments();
    auto ann_segs = d_2h_->GetAnnulusSegments();
    EXPECT_EQ(4, tub_segs.size()); // Includes root segment
    EXPECT_EQ(3, icd_segs.size());
    EXPECT_EQ(segs.size() - 7, ann_segs.size());

    for (int i = 1; i < tub_segs.size(); ++i) {
        EXPECT_EQ(tub_segs[i].Outlet(), tub_segs[i-1].Index());

        // Check that the next tub index is in the list of inlets for the prev one.
        EXPECT_TRUE(std::find(tub_segs[i-1].GetInlets().begin(),
                              tub_segs[i-1].GetInlets().end(),
                              tub_segs[i].Index()) != tub_segs[i-1].GetInlets().end());
    }
    for (int i = 0; i < icd_segs.size(); ++i) {
        EXPECT_EQ(icd_segs[i].Outlet(), tub_segs[i].Index());
    }
    for (auto seg : segs) {
        std::cout << seg.ToString();
    }
}

}
