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

#ifndef FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H
#define FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H

#include "Model/properties/variable_property_container.h"

using namespace Model::Properties;

namespace TestResources {
    class TestResourceVariablePropertyContainer {

    public:
        TestResourceVariablePropertyContainer() {
            prod_heel_x_ = new ContinousProperty(5.0);
            prod_heel_y_ = new ContinousProperty(5.0);
            prod_heel_z_ = new ContinousProperty(200.0);
            prod_toe_x_ = new ContinousProperty(12);
            prod_toe_y_ = new ContinousProperty(120.1);
            prod_toe_z_ = new ContinousProperty(300.0);
            prod_heel_x_->setName("SplinePoint#TESTW#heel#x");
            prod_heel_y_->setName("SplinePoint#TESTW#heel#y");
            prod_heel_z_->setName("SplinePoint#TESTW#heel#z");
            prod_toe_x_->setName("SplinePoint#TESTW#toe#x");
            prod_toe_y_->setName("SplinePoint#TESTW#toe#y");
            prod_toe_z_->setName("SplinePoint#TESTW#toe#z");
            varcont_prod_spline_ = new VariablePropertyContainer();
            varcont_prod_spline_->AddVariable(prod_heel_x_);
            varcont_prod_spline_->AddVariable(prod_heel_y_);
            varcont_prod_spline_->AddVariable(prod_heel_z_);
            varcont_prod_spline_->AddVariable(prod_toe_x_);
            varcont_prod_spline_->AddVariable(prod_toe_y_);
            varcont_prod_spline_->AddVariable(prod_toe_z_);

            prod_bhp_0_ = new ContinousProperty(1.0);
            prod_bhp_10_ = new ContinousProperty(1.0);
            prod_bhp_0_->setName("BHP#PRODUCER#0");
            prod_bhp_10_->setName("BHP#PRODUCER#10");
            varcont_prod_bhp_ = new VariablePropertyContainer();
            varcont_prod_bhp_->AddVariable(prod_bhp_0_);
            varcont_prod_bhp_->AddVariable(prod_bhp_10_);



        }

        VariablePropertyContainer *varcont_prod_spline_;
        VariablePropertyContainer *varcont_prod_bhp_;
        VariablePropertyContainer *varcont_6r_;

    private:
        ContinousProperty *prod_heel_x_;
        ContinousProperty *prod_heel_y_;
        ContinousProperty *prod_heel_z_;
        ContinousProperty *prod_toe_x_;
        ContinousProperty *prod_toe_y_;
        ContinousProperty *prod_toe_z_;

        ContinousProperty *prod_bhp_0_;
        ContinousProperty *prod_bhp_10_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H
