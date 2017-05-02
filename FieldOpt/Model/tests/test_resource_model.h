/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef FIELDOPT_TEST_RESOURCE_MODEL_H
#define FIELDOPT_TEST_RESOURCE_MODEL_H

#include <Runner/tests/test_resource_runner.hpp>
#include "Model/model.h"
#include "Settings/tests/test_resource_settings.hpp"

namespace TestResources {
class TestResourceModel : public TestResourceSettings,
                          public TestResources::RunnerResources
{
 protected:
  TestResourceModel() {
      settings_model_->set_reservoir_grid_path(ExampleFilePaths::grid_5spot_);
      model_ = new Model::Model(*settings_model_, logger_);
  }

  Model::Model *model_;
};
}

#endif //FIELDOPT_TEST_RESOURCE_MODEL_H
