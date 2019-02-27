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

#ifndef FIELDOPT_TEST_RESOURCE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SETTINGS_H

#include "Settings/settings.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace TestResources {

class TestResourceSettings {
 protected:
  TestResourceSettings() {
      paths_.SetPath(Paths::DRIVER_FILE, ExampleFilePaths::driver_example_);
      paths_.SetPath(Paths::OUTPUT_DIR, ExampleFilePaths::directory_output_);
      paths_.SetPath(Paths::SIM_DRIVER_FILE, ExampleFilePaths::deck_horzwel_);
      paths_.SetPath(Paths::GRID_FILE, ExampleFilePaths::grid_5spot_);
      settings_full_ = new Settings::Settings(paths_);
      settings_optimizer_ = settings_full_->optimizer();
      settings_simulator_ = settings_full_->simulator();
      settings_model_ = settings_full_->model();

      paths_hybridopt_.SetPath(Paths::DRIVER_FILE, ExampleFilePaths::hybridopt_driver_example_);
      paths_hybridopt_.SetPath(Paths::OUTPUT_DIR, ExampleFilePaths::directory_output_);
      paths_hybridopt_.SetPath(Paths::SIM_DRIVER_FILE, ExampleFilePaths::deck_horzwel_);
      paths_hybridopt_.SetPath(Paths::GRID_FILE, ExampleFilePaths::grid_5spot_);
      settings_hybridopt_full_ = new Settings::Settings(paths_hybridopt_);
      settings_hybridopt_optimizer_ = settings_hybridopt_full_->optimizer();
      settings_hybridopt_simulator_ = settings_hybridopt_full_->simulator();
      settings_hybridopt_model_ = settings_hybridopt_full_->model();
  }

  Settings::Settings *settings_full_;
  Settings::Optimizer *settings_optimizer_;
  Settings::Simulator *settings_simulator_;
  Settings::Model *settings_model_;
  Paths paths_;

  Settings::Settings *settings_hybridopt_full_;
  Settings::Optimizer *settings_hybridopt_optimizer_;
  Settings::Simulator *settings_hybridopt_simulator_;
  Settings::Model *settings_hybridopt_model_;
  Paths paths_hybridopt_;
};

}

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
