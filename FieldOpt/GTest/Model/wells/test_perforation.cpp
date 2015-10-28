/******************************************************************************
 *
 *
 *
 * Created: 28.10.2015 2015 by einar
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
#include "Model/wells/completions/perforation.h"

using namespace Model::Wells;

namespace {

class PerforationTest : public ::testing::Test {
protected:
    PerforationTest() {
        settings_ = new ::Utilities::Settings::Settings(driver_file_path_);
        variable_container_ = new ::Model::Variables::VariableContainer();
        variable_handler_ = new ::Model::Variables::VariableHandler(*settings_->model());
        prod_well_ = settings_->model()->wells().first();
    }
    virtual ~PerforationTest() {}
    virtual void SetUp() {
        prod_perforations_ = QList<Completions::Perforation *>();
        for (int i = 0; i < prod_well_.completions.size(); ++i) {
            prod_perforations_.append(new Completions::Perforation(prod_well_.completions[i],
                                                                   variable_container_,
                                                                   variable_handler_));
        }
    }

    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    ::Utilities::Settings::Settings *settings_;
    ::Model::Variables::VariableHandler *variable_handler_;
    ::Model::Variables::VariableContainer *variable_container_;
    ::Utilities::Settings::Model::Well prod_well_;
    QList<Completions::Perforation *> prod_perforations_;
};

TEST_F(PerforationTest, Constructor) {
    EXPECT_EQ(2, prod_perforations_.size());
}

TEST_F(PerforationTest, Values) {
    EXPECT_FLOAT_EQ(1.0, prod_perforations_[0]->transmissibility_factor());
    EXPECT_FLOAT_EQ(1.0, prod_perforations_[1]->transmissibility_factor());

    EXPECT_EQ(1, prod_perforations_[0]->i());
    EXPECT_EQ(4, prod_perforations_[0]->j());
    EXPECT_EQ(2, prod_perforations_[0]->k());

    EXPECT_EQ(2, prod_perforations_[1]->i());
    EXPECT_EQ(4, prod_perforations_[1]->j());
    EXPECT_EQ(2, prod_perforations_[1]->k());
}

TEST_F(PerforationTest, VariableHandlerCorrectness) {
    EXPECT_TRUE(variable_handler_->GetPerforation(0)->transmissibility_factor());
    EXPECT_TRUE(variable_handler_->GetPerforation(1)->transmissibility_factor());
}

TEST_F(PerforationTest, VariableContainerConsistencyAfterCreation) {
    EXPECT_FLOAT_EQ(1.0, variable_container_->GetRealVariable(0)->value());
    EXPECT_FLOAT_EQ(1.0, variable_container_->GetRealVariable(1)->value());
}

TEST_F(PerforationTest, VariableContainerConsistencyAfterModification) {
    // Change values directly and verify change in variable container
    prod_perforations_[0]->setTransmissibility_factor(2.0);
    prod_perforations_[1]->setTransmissibility_factor(3.0);
    EXPECT_FLOAT_EQ(2.0, variable_container_->GetRealVariable(0)->value());
    EXPECT_FLOAT_EQ(3.0, variable_container_->GetRealVariable(1)->value());

    // Change value in variable container and verify change directly
    variable_container_->GetRealVariable(0)->setValue(4.0);
    variable_container_->GetRealVariable(1)->setValue(5.0);
    EXPECT_FLOAT_EQ(4.0, prod_perforations_[0]->transmissibility_factor());
    EXPECT_FLOAT_EQ(5.0, prod_perforations_[1]->transmissibility_factor());

}

}
