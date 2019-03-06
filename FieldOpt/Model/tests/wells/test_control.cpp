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

#include <gtest/gtest.h>
#include <tests/test_resource_model.h>
#include "Model/wells/control.h"
#include "Model/tests/test_resource_model.h"

using namespace Model::Wells;

namespace {

class ControlTest : public testing::Test, public TestResources::TestResourceModel {
protected:
    ControlTest() {
        well_ = settings_model_->wells().first();
        inj_well_ = settings_model_->wells()[1];
        entry_ = well_.controls.first();
        inj_entry_ = inj_well_.controls.first();

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < settings_model_->wells()[i].controls.size(); ++j) {
                all_controls_.append(new Control(settings_model_->wells()[i].controls[j],
                                                 settings_model_->wells()[i],
                                                 model_->variables()));
            }
        }
    }
    virtual ~ControlTest() {}
    virtual void SetUp() {}

    ::Settings::Model::Well well_;
    ::Settings::Model::Well inj_well_;
    ::Settings::Model::Well::ControlEntry entry_;
    ::Settings::Model::Well::ControlEntry inj_entry_;
    QList<Control *> all_controls_;
};

TEST_F(ControlTest, ProducerControl) {

    EXPECT_STREQ("PROD", well_.name.toLatin1().constData());
    EXPECT_EQ(0, entry_.time_step);
    EXPECT_TRUE(all_controls_.first()->open());
    EXPECT_EQ(::Settings::Model::ControlMode::BHPControl, all_controls_.first()->mode());
    EXPECT_FLOAT_EQ(100, all_controls_.first()->bhp());
}

TEST_F(ControlTest, InjectorControl) {
    EXPECT_STREQ("INJ", inj_well_.name.toLatin1().constData());
    EXPECT_EQ(0, inj_entry_.time_step);
    EXPECT_EQ(0, all_controls_.last()->time_step());
    EXPECT_TRUE(all_controls_.last()->open());
    EXPECT_EQ(::Settings::Model::ControlMode::LRATControl, all_controls_.last()->mode());
    EXPECT_FLOAT_EQ(1200, all_controls_.last()->rate());
}

}
