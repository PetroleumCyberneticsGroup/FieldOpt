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
      settings_full_ = new Settings::Settings(
          ExampleFilePaths::driver_example_,
          ExampleFilePaths::directory_output_,
          ExampleFilePaths::include_dir_);
      settings_optimizer_ = settings_full_->optimizer();
      settings_simulator_ = settings_full_->simulator();
      settings_model_ = settings_full_->model();

//      settings_flow_5spot_ = new Settings::Settings(ExampleFilePaths::driver_5spot_flow_,
//                                                    "/home/einar/.CLion2016.2/system/cmake/generated/FieldOpt-c9373114/c9373114/Debug/fieldopt_output/");
//      settings_flow_5spot_->simulator()->set_driver_file_path(TestResources::ExampleFilePaths::flow_drv_5spot_);
//      settings_flow_5spot_->model()->set_reservoir_grid_path(ExampleFilePaths::grid_5spot_flow_);
  }

  Settings::Settings *settings_full_;
  Settings::Optimizer *settings_optimizer_;
  Settings::Simulator *settings_simulator_;
  Settings::Model *settings_model_;
//        Settings::Settings *settings_flow_5spot_;
};

}

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
