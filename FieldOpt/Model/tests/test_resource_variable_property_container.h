//
// Created by einar on 5/27/16.
//

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
            prod_heel_x_->setName("SplinePoint#PRODUCER#heel#x");
            prod_heel_y_->setName("SplinePoint#PRODUCER#heel#y");
            prod_heel_z_->setName("SplinePoint#PRODUCER#heel#z");
            prod_toe_x_->setName("SplinePoint#PRODUCER#toe#x");
            prod_toe_y_->setName("SplinePoint#PRODUCER#toe#y");
            prod_toe_z_->setName("SplinePoint#PRODUCER#toe#z");
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
