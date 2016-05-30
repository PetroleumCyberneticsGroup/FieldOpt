#ifndef FIELDOPT_TEST_RESOURCE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SETTINGS_H

#include "Utilities/settings/settings.h"

class TestResourceSettings {
protected:
    TestResourceSettings() {
        full_settings_ = new Utilities::Settings::Settings(driver_file_path_, output_directory_);
        optimizer_settings_ = full_settings_->optimizer();
        simulator_settings_ = full_settings_->simulator();
        model_settings_ = full_settings_->model();

        constraint_settings_reservoir_boundary_.name = "BOUNDARY_CONSTRAINT";
        constraint_settings_reservoir_boundary_.type = Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary;
        constraint_settings_reservoir_boundary_.imin = 1;
        constraint_settings_reservoir_boundary_.imax = 5;
        constraint_settings_reservoir_boundary_.jmin = 1;
        constraint_settings_reservoir_boundary_.jmax = 5;
        constraint_settings_reservoir_boundary_.kmin = 1;
        constraint_settings_reservoir_boundary_.kmax = 5;
        constraint_settings_reservoir_boundary_.wells << "PRODUCER" << "INJECTOR";
    }
    Utilities::Settings::Settings *full_settings_;
    Utilities::Settings::Optimizer *optimizer_settings_;
    Utilities::Settings::Simulator *simulator_settings_;
    Utilities::Settings::Model *model_settings_;

    QString driver_file_path_ = "../examples/driver.json";
    QString output_directory_ = "../fieldopt_output";

    Utilities::Settings::Optimizer::Constraint constraint_settings_reservoir_boundary_;
};

#endif //FIELDOPT_TEST_RESOURCE_SETTINGS_H
