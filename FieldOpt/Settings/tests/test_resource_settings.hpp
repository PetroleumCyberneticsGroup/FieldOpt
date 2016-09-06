#ifndef FIELDOPT_TEST_RESOURCE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SETTINGS_H

#include "Settings/settings.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

namespace TestResources {

    class TestResourceSettings {
    protected:
        TestResourceSettings() {
            settings_full_ = new Utilities::Settings::Settings(ExampleFilePaths::driver_example_, ExampleFilePaths::directory_output_);
            settings_optimizer_ = settings_full_->optimizer();
            settings_simulator_ = settings_full_->simulator();
            settings_model_ = settings_full_->model();

//            settings_flow_5spot_ = new Utilities::Settings::Settings(ExampleFilePaths::driver_5spot_flow_,
//                                                                     "/home/einar/.CLion2016.2/system/cmake/generated/FieldOpt-c9373114/c9373114/Debug/fieldopt_output/");
//            settings_flow_5spot_->simulator()->set_driver_file_path(TestResources::ExampleFilePaths::flow_drv_5spot_);
//            settings_flow_5spot_->model()->set_reservoir_grid_path(ExampleFilePaths::grid_5spot_flow_);

            constraint_settings_reservoir_boundary_.type = Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary;
            constraint_settings_reservoir_boundary_.box_imin = 1;
            constraint_settings_reservoir_boundary_.box_imax = 1;
            constraint_settings_reservoir_boundary_.box_jmin = 0;
            constraint_settings_reservoir_boundary_.box_jmax = 0;
            constraint_settings_reservoir_boundary_.box_kmin = 0;
            constraint_settings_reservoir_boundary_.box_kmax = 0;
            constraint_settings_reservoir_boundary_.well = "PRODUCER";
        }

        Utilities::Settings::Settings *settings_full_;
        Utilities::Settings::Optimizer *settings_optimizer_;
        Utilities::Settings::Simulator *settings_simulator_;
        Utilities::Settings::Model *settings_model_;
//        Utilities::Settings::Settings *settings_flow_5spot_;

        Utilities::Settings::Optimizer::Constraint constraint_settings_reservoir_boundary_;
    };

}

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
