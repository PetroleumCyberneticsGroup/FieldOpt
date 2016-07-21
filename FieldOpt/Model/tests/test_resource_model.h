//
// Created by einar on 6/2/16.
//

#ifndef FIELDOPT_TEST_RESOURCE_MODEL_H
#define FIELDOPT_TEST_RESOURCE_MODEL_H

#include "Model/model.h"
#include "Utilities/tests/test_resource_settings.h"
#include <iostream>

namespace TestResources {
    class TestResourceModel : public TestResourceSettings {
    protected:
        TestResourceModel() {
            // ESTABLISH MODEL OBJECT FOR TESTING
            if (settings_full_->verbosity()>2) std::cout << "... establishing model object for testing (test_resource_model.h)" << std::endl;
            settings_model_->set_reservoir_grid_path(ExampleFilePaths::grid_5spot_);
            model_ = new Model::Model(*settings_model_);
        }

        Model::Model *model_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_MODEL_H
