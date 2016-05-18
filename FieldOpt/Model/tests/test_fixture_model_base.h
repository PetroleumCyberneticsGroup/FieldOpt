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

class ModelBaseTest : public ::testing::Test {
protected:
    ModelBaseTest() {
        settings_ = new ::Utilities::Settings::Settings(driver_file_path_, output_directory_);
        settings_->model()->set_reservoir_grid_path(reservoir_grid_path_);
        variable_container_ = new ::Model::Properties::VariablePropertyContainer();
        variable_handler_ = new ::Model::Properties::VariablePropertyHandler(*settings_->model());
        model_ = new ::Model::Model(*settings_->model());
    }

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
//    QString reservoir_grid_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
    QString reservoir_grid_path_ = Utilities::FileHandling::GetBuildDirectoryPath() + "/../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
    ::Utilities::Settings::Settings *settings_;
    ::Model::Properties::VariablePropertyContainer *variable_container_;
    ::Model::Properties::VariablePropertyHandler *variable_handler_;
    ::Model::Model *model_;
};

#endif // TEST_FIXTURE_MODEL_BASE
