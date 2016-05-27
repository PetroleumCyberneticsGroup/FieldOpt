//
// Created by einar on 5/27/16.
//

#ifndef FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H
#define FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H

#include "Model/properties/variable_property_container.h"

class TestResourceVariablePropertyContainer {

public:
    TestResourceVariablePropertyContainer() {
        prod_heel_x_ = new Model::Properties::ContinousProperty(5.0);
        prod_heel_y_ = new Model::Properties::ContinousProperty(5.0);
        prod_heel_z_ = new Model::Properties::ContinousProperty(5.0);
        prod_toe_x_ = new Model::Properties::ContinousProperty(10.0);
        prod_toe_y_ = new Model::Properties::ContinousProperty(10.0);
        prod_toe_z_ = new Model::Properties::ContinousProperty(10.0);
        prod_heel_x_->setName("SplinePoints#PRODUCER#heel#x");
        prod_heel_y_->setName("SplinePoints#PRODUCER#heel#y");
        prod_heel_z_->setName("SplinePoints#PRODUCER#heel#z");
        prod_toe_x_->setName("SplinePoints#PRODUCER#toe#x");
        prod_toe_y_->setName("SplinePoints#PRODUCER#toe#y");
        prod_toe_z_->setName("SplinePoints#PRODUCER#toe#z");

        variable_property_container_ = new Model::Properties::VariablePropertyContainer();
        variable_property_container_->AddVariable(prod_heel_x_);
        variable_property_container_->AddVariable(prod_heel_y_);
        variable_property_container_->AddVariable(prod_heel_z_);
        variable_property_container_->AddVariable(prod_toe_x_);
        variable_property_container_->AddVariable(prod_toe_y_);
        variable_property_container_->AddVariable(prod_toe_z_);
    }

    Model::Properties::VariablePropertyContainer *variable_property_container_;

private:
    Model::Properties::ContinousProperty *prod_heel_x_;
    Model::Properties::ContinousProperty *prod_heel_y_;
    Model::Properties::ContinousProperty *prod_heel_z_;
    Model::Properties::ContinousProperty *prod_toe_x_;
    Model::Properties::ContinousProperty *prod_toe_y_;
    Model::Properties::ContinousProperty *prod_toe_z_;
};

#endif //FIELDOPT_TEST_RESOURCE_VARIABLE_PROPERTY_LIST_H
