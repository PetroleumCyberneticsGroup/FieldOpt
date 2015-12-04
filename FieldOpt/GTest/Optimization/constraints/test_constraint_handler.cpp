/******************************************************************************
 *
 *
 *
 * Created: 04.12.2015 2015 by einar
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

#include "GTest/Optimization/test_fixture_case.h"
#include "Optimization/constraints/constraint_handler.h"
#include <QString>

namespace {

class ConstraintHandlerTest : public CaseTestFixture {
protected:
    ConstraintHandlerTest() {
        constraint_handler_ = new ::Optimization::Constraints::ConstraintHandler(optimizer_settings_->constraints(), model_->variables());
    }
    virtual ~ConstraintHandlerTest() {}
    virtual void SetUp() {}

    Optimization::Constraints::ConstraintHandler *constraint_handler_;
};

TEST_F(ConstraintHandlerTest, Constructor) {
    EXPECT_EQ(1, constraint_handler_->constraints().size());
    EXPECT_STREQ(constraint_handler_->constraints().first()->name().toLatin1().constData(), "PROD-BHP-1");
}

TEST_F(ConstraintHandlerTest, BaseCaseSatasfaction) {
    EXPECT_TRUE(constraint_handler_->constraints().first()->CaseSatisfiesConstraint(base_case_));
}

}
