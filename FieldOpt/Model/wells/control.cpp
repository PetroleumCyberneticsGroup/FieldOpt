#include "control.h"

namespace Model {
    namespace Wells {

        Control::Control(::Settings::Model::Well::ControlEntry entry,
                         ::Settings::Model::Well well,
                         ::Model::Properties::VariablePropertyContainer *variables)
        {
            time_step_ = new Properties::DiscreteProperty(entry.time_step);

            if (well.type == ::Settings::Model::WellType::Injector)
                injection_fluid_ = entry.injection_type;

            // Open/Closed
            if (entry.state == ::Settings::Model::WellState::WellOpen)
                open_ = new Properties::BinaryProperty(true);
            else if (entry.state == ::Settings::Model::WellState::WellShut)
                open_ = new Properties::BinaryProperty(false);

            switch (entry.control_mode) {
                case ::Settings::Model::ControlMode::BHPControl:
                    mode_ = entry.control_mode;
                    bhp_ = new Properties::ContinousProperty(entry.bhp);
                    if (entry.is_variable) {
                        bhp_->setName(entry.name);
                        variables->AddVariable(bhp_);
                    }
                    // If set, this will be passed as the upper rate limit to the simulator
                    rate_ = new Properties::ContinousProperty(entry.rate);
                    break;
                case ::Settings::Model::ControlMode::RateControl:
                    mode_ = entry.control_mode;
                    rate_ = new Properties::ContinousProperty(entry.rate);
                    if (entry.is_variable) {
                        rate_->setName(entry.name);
                        variables->AddVariable(rate_);
                    }
                    // If set, this will be passed as the lower (upper for injector) BHP limit to the simulator
                    bhp_ = new Properties::ContinousProperty(entry.bhp);
            }

        }

    }
}
