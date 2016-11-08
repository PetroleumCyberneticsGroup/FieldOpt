#include <iostream>
#include "bhp_constraint.h"

namespace Optimization {
    namespace Constraints {

        BhpConstraint::BhpConstraint(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables)
        {
            min_ = settings.min;
            max_ = settings.max;
            affected_well_names_ = settings.wells;
            for (auto wname : affected_well_names_) {
                affected_real_variables_.append(variables->GetWellBHPVariables(wname));
            }
        }

        bool BhpConstraint::CaseSatisfiesConstraint(Case *c)
        {
            for (auto var : affected_real_variables_) {
                double case_value = c->real_variables()[var->id()];
                if (case_value > max_ || case_value < min_)
                    return false;
            }
            return true;
        }

        void BhpConstraint::SnapCaseToConstraints(Case *c)
        {
            for (auto var : affected_real_variables_) {
                if (c->real_variables()[var->id()] > max_)
                    c->set_real_variable_value(var->id(), max_);
                else if (c->real_variables()[var->id()] < min_)
                    c->set_real_variable_value(var->id(), min_);
            }
        }

    }
}
