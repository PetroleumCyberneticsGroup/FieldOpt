#ifndef FIELDOPT_TEST_RESOURCE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SETTINGS_H

#include "Utilities/settings/settings.h"
#include "Utilities/tests/test_resource_example_file_paths.h"

namespace TestResources {

    class TestResourceSettings {
    protected:
        TestResourceSettings() {
                settings_full_ = new Utilities::Settings::Settings(ExampleFilePaths::driver_example_, ExampleFilePaths::directory_output_);
                settings_optimizer_ = settings_full_->optimizer();
                settings_simulator_ = settings_full_->simulator();
                settings_model_ = settings_full_->model();

                constraint_settings_reservoir_boundary_.type = Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary;
                constraint_settings_reservoir_boundary_.imin = 1;
                constraint_settings_reservoir_boundary_.imax = 1;
                constraint_settings_reservoir_boundary_.jmin = 0;
                constraint_settings_reservoir_boundary_.jmax = 0;
                constraint_settings_reservoir_boundary_.kmin = 0;
                constraint_settings_reservoir_boundary_.kmax = 0;
                constraint_settings_reservoir_boundary_.well = "PRODUCER";
        }

        Utilities::Settings::Settings *settings_full_;
        Utilities::Settings::Optimizer *settings_optimizer_;
        Utilities::Settings::Simulator *settings_simulator_;
        Utilities::Settings::Model *settings_model_;

        Utilities::Settings::Optimizer::Constraint constraint_settings_reservoir_boundary_;
    };

}

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
