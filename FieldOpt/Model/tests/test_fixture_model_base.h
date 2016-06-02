/******************************************************************************
 *
 *
 *
 * Created: 10.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef TEST_FIXTURE_MODEL_BASE
#define TEST_FIXTURE_MODEL_BASE

#include <gtest/gtest.h>
#include "Utilities/settings/settings.h"
#include "Utilities/file_handling/filehandling.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/variable_property_handler.h"
#include "Model/model.h"
#include "Model/tests/test_resource_grids.h"
#include "Utilities/tests/test_resource_settings.h"
#include "Model/tests/test_resource_model.h"

class ModelBaseTest : public ::testing::Test, TestResources::TestResourceGrids, TestResources::TestResourceSettings {
protected:
    ModelBaseTest() {
        settings_ = settings_full_;
        settings_->model()->set_reservoir_grid_path(TestResources::ExampleFilePaths::grid_5spot_);
        variable_container_ = new ::Model::Properties::VariablePropertyContainer();
        variable_handler_ = new ::Model::Properties::VariablePropertyHandler(*settings_->model());
        model_ = new ::Model::Model(*settings_->model());
    }

    ::Utilities::Settings::Settings *settings_;
    ::Model::Properties::VariablePropertyContainer *variable_container_;
    ::Model::Properties::VariablePropertyHandler *variable_handler_;
    ::Model::Model *model_;
};

#endif // TEST_FIXTURE_MODEL_BASE
