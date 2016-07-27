#ifndef FIELDOPT_TEST_RESOURCE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SETTINGS_H

#include "Utilities/settings/settings.h"
#include "Utilities/tests/test_resource_example_file_paths.h"
#include <iostream>

namespace TestResources {

    class TestResourceSettings {
    protected:
        TestResourceSettings() {

            settings_full_ = new Utilities::Settings::Settings(
                    ExampleFilePaths::driver_example_,
                    ExampleFilePaths::directory_output_);
            settings_optimizer_ = settings_full_->optimizer();
            settings_simulator_ = settings_full_->simulator();
            settings_model_ = settings_full_->model();

            // ESTABLISH OVERALL SETTINGS FOR TESTING
            if (settings_full_->verbosity()>2) std::cout << "... establishing overall settings for testing (test_resource_settings.h)" << std::endl;

//            settings_flow_5spot_ = new Utilities::Settings::Settings(ExampleFilePaths::driver_5spot_flow_,
//                                                                     "/home/einar/.CLion2016.2/system/cmake/generated/FieldOpt-c9373114/c9373114/Debug/fieldopt_output/");
//            settings_flow_5spot_->simulator()->set_driver_file_path(TestResources::ExampleFilePaths::flow_drv_5spot_);
//            settings_flow_5spot_->model()->set_reservoir_grid_path(ExampleFilePaths::grid_5spot_flow_);

        }

        Utilities::Settings::Settings *settings_full_;
        Utilities::Settings::Optimizer *settings_optimizer_;
        Utilities::Settings::Simulator *settings_simulator_;
        Utilities::Settings::Model *settings_model_;
//        Utilities::Settings::Settings *settings_flow_5spot_;

    };

}

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
