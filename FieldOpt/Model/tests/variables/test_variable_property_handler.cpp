/******************************************************************************
 *
 *
 *
 * Created: 21.10.2015 2015 by einar
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

#include <gtest/gtest.h>
#include "Model/properties/variable_property_handler.h"
#include "Model/properties/property_exceptions.h"

using namespace Model::Properties;

namespace {

class VariableHandlerTest : public ::testing::Test {
protected:
    VariableHandlerTest() {
        settings_ = new Utilities::Settings::Settings(driver_file_path_, output_directory_);
    }

    ~VariableHandlerTest() {}

    virtual void SetUp() {
        variable_handler_ = new VariablePropertyHandler(*settings_->model());
    }
    virtual void TearDown() {
        delete variable_handler_;
    }

    Utilities::Settings::Settings *settings_;
    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
    VariablePropertyHandler *variable_handler_;
};

TEST_F(VariableHandlerTest, Wells) {
    EXPECT_STREQ("PROD", variable_handler_->GetWell("PROD")->name().toLatin1().constData());
    EXPECT_STREQ("INJ", variable_handler_->GetWell("INJ")->name().toLatin1().constData());
    EXPECT_THROW(variable_handler_->GetWell("NOWELL"), VariablePropertyHandlerCannotFindObjectException);
}

TEST_F(VariableHandlerTest, ProducerControls) {
    QList<int> control_times = QList<int> {0, 50, 100, 365};

    // All these should be defaulted to false (they are not set to variable at any time step)
    foreach (int control_time, control_times) {
        EXPECT_FALSE(variable_handler_->GetControl("PROD", control_time)->open());
        EXPECT_FALSE(variable_handler_->GetControl("PROD", control_time)->rate());
        EXPECT_FALSE(variable_handler_->GetControl("INJ", control_time)->open());
        EXPECT_FALSE(variable_handler_->GetControl("INJ", control_time)->rate());
        EXPECT_FALSE(variable_handler_->GetControl("INJ", control_time)->bhp());
    }
    EXPECT_THROW(variable_handler_->GetControl("PROD", 10)->open(), VariablePropertyHandlerCannotFindObjectException);
    EXPECT_THROW(variable_handler_->GetControl("INJ", 15)->open(), VariablePropertyHandlerCannotFindObjectException);

    // These have been changed for some time steps
    EXPECT_TRUE(variable_handler_->GetControl("PROD", 0)->bhp());
    EXPECT_TRUE(variable_handler_->GetControl("PROD", 50)->bhp());
    EXPECT_TRUE(variable_handler_->GetControl("PROD", 365)->bhp());

}

TEST_F(VariableHandlerTest, PerforationVariables) {
    EXPECT_TRUE(variable_handler_->GetPerforation(0)->transmissibility_factor());
    EXPECT_TRUE(variable_handler_->GetPerforation(1)->transmissibility_factor());
    EXPECT_THROW(variable_handler_->GetPerforation(3), VariablePropertyHandlerCannotFindObjectException);
}

TEST_F(VariableHandlerTest, WellBlockVariables) {
    EXPECT_TRUE(variable_handler_->GetWellBlock(0)->position());
    EXPECT_TRUE(variable_handler_->GetWellBlock(1)->position());
    EXPECT_TRUE(variable_handler_->GetWellBlock(2)->position());
    EXPECT_TRUE(variable_handler_->GetWellBlock(3)->position());
    EXPECT_THROW(variable_handler_->GetWellBlock(4)->position(), VariablePropertyHandlerCannotFindObjectException);
}


}

