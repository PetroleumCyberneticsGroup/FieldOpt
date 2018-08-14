#include "rate_constraint.h"

namespace Optimization {
    namespace Constraints {
        RateConstraint::RateConstraint(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables) {

            assert(settings.wells.size() > 0);
            assert(settings.min < settings.max);

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
    Eigen::VectorXd RateConstraint::GetLowerBounds(QList<QUuid> id_vector) const {
        Eigen::VectorXd lbounds(id_vector.size());
        lbounds.fill(0);
        for (auto var : affected_real_variables_) {
            lbounds[id_vector.indexOf(var->id())] = min_;
        }
        return lbounds;
    }
    Eigen::VectorXd RateConstraint::GetUpperBounds(QList<QUuid> id_vector) const {
        Eigen::VectorXd ubounds(id_vector.size());
        ubounds.fill(0);
        for (auto var : affected_real_variables_) {
            ubounds[id_vector.indexOf(var->id())] = max_;
        }
        return ubounds;
    }
    bool RateConstraint::IsBoundConstraint() const {
        return true;
    }

    }
}
