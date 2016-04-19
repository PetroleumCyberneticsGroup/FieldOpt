/******************************************************************************
 *
 *
 *
 * Created: 22.10.2015 2015 by einar
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

#include "GTest/Model/test_fixture_model_base.h"
#include "Model/wells/control.h"

using namespace Model::Wells;

namespace {

class ControlTest : public ModelBaseTest {
protected:
    ControlTest() {
        well_ = settings_->model()->wells().first();
        inj_well_ = settings_->model()->wells().last();
        entry_ = well_.controls.first();
        inj_entry_ = inj_well_.controls.first();

        for (int i = 0; i < settings_->model()->wells().size(); ++i) {
            for (int j = 0; j < settings_->model()->wells()[i].controls.size(); ++j) {
                all_controls_.append(new Control(settings_->model()->wells()[i].controls[j],
                                                 settings_->model()->wells()[i],
                                                 variable_container_,
                                                 variable_handler_));
            }
        }
    }
    virtual ~ControlTest() {}
    virtual void SetUp() {}

    ::Utilities::Settings::Model::Well well_;
    ::Utilities::Settings::Model::Well inj_well_;
    ::Utilities::Settings::Model::ControlEntry entry_;
    ::Utilities::Settings::Model::ControlEntry inj_entry_;
    QList<Control *> all_controls_;
};

TEST_F(ControlTest, ProducerControl) {

    EXPECT_STREQ("PROD", well_.name.toLatin1().constData());
    EXPECT_EQ(0, entry_.time_step);
    EXPECT_TRUE(all_controls_.first()->open());
    EXPECT_EQ(::Utilities::Settings::Model::ControlMode::BHPControl, all_controls_.first()->mode());
    EXPECT_FLOAT_EQ(2000, all_controls_.first()->bhp());
}

TEST_F(ControlTest, InjectorControl) {
    EXPECT_STREQ("INJ", inj_well_.name.toLatin1().constData());
    EXPECT_EQ(0, inj_entry_.time_step);
    EXPECT_EQ(0, all_controls_.last()->time_step());
    EXPECT_TRUE(all_controls_.last()->open());
    EXPECT_EQ(::Utilities::Settings::Model::ControlMode::RateControl, all_controls_.last()->mode());
    EXPECT_FLOAT_EQ(1200, all_controls_.last()->rate());
}

TEST_F(ControlTest, VariableContainerConsistencyAfterCreation) {
    EXPECT_EQ(0, variable_container_->BinaryVariableSize());
    EXPECT_EQ(0, variable_container_->DiscreteVariableSize());
    EXPECT_EQ(3, variable_container_->ContinousVariableSize()); // BHP for PROD is set variable at three time steps
}

}
