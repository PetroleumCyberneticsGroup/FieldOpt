//
// Created by einar on 6/6/18.
//

#include <gtest/gtest.h>
#include <wells/well.h>
#include "Settings/settings.h"
#include "Settings/tests/test_resource_schedule_import_settings.hpp"
#include "Settings/paths.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace {

class BlockToSplineConversionTest : public ::testing::Test {
 protected:
  BlockToSplineConversionTest() {
      auto partial_deck = TestResources::TestResourceSettings::imported_model_settings;
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

TEST_F(BlockToSplineConversionTest, Constructor ) {
    // Verifying settings
    d_2h_ = new Model::Wells::Well(*mod_settings_, d_2h_idx_, varcont_, grid_, nullptr);
}

}
