#include "rate_constraint.h"

namespace Optimization {
    namespace Constraints {
        RateConstraint::RateConstraint(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables) {
            affected_well_names_ = settings.wells;
            min_ = settings.min;
            max_ = settings.max;
            penalty_weight_ = settings.penalty_weight;
            for (auto wname : affected_well_names_) {
                affected_real_variables_.append(variables->GetWellRateVariables(wname));
            }
        }

        bool RateConstraint::CaseSatisfiesConstraint(Case *c) {
            for (auto var : affected_real_variables_) {
                double case_value = c->real_variables()[var->id()];
                if (case_value > max_ || case_value < min_)
                    return false;
            }
            return true;
        }

        void RateConstraint::SnapCaseToConstraints(Case *c) {
            for (auto var : affected_real_variables_) {
                if (c->real_variables()[var->id()] > max_)
                    c->set_real_variable_value(var->id(), max_);
                else if (c->real_variables()[var->id()] < min_)
                    c->set_real_variable_value(var->id(), min_);
            }
        }

    }
}
