#include "control.h"

namespace Model {
    namespace Wells {

        Control::Control(::Utilities::Settings::Model::Well::ControlEntry entry,
                         ::Utilities::Settings::Model::Well well,
                         ::Model::Properties::VariablePropertyContainer *variables)
        {
            time_step_ = new Properties::DiscreteProperty(entry.time_step);

            if (well.type == ::Utilities::Settings::Model::WellType::Injector)
                injection_fluid_ = entry.injection_type;

            // Open/Closed
            if (entry.state == ::Utilities::Settings::Model::WellState::WellOpen)
                open_ = new Properties::BinaryProperty(true);
            else if (entry.state == ::Utilities::Settings::Model::WellState::WellShut)
                open_ = new Properties::BinaryProperty(false);

            switch (entry.control_mode) {
                case ::Utilities::Settings::Model::ControlMode::BHPControl:
                    mode_ = entry.control_mode;
                    bhp_ = new Properties::ContinousProperty(entry.bhp);
                    if (entry.is_variable) {
                        bhp_->setName(entry.name);
                        variables->AddVariable(bhp_);
                    }
                    break;
                case ::Utilities::Settings::Model::ControlMode::RateControl:
                    mode_ = entry.control_mode;
                    rate_ = new Properties::ContinousProperty(entry.rate);
                    if (entry.is_variable) {
                        rate_->setName(entry.name);
                        variables->AddVariable(rate_);
                    }
            }

        }

    }
}
