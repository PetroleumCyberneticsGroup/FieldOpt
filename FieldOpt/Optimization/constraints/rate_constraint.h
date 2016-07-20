#ifndef FIELDOPT_RATE_CONSTRAINT_H
#define FIELDOPT_RATE_CONSTRAINT_H

#include "constraint.h"

namespace Optimization {
    namespace Constraints {
        class RateConstraint : public Constraint {
        public:
            RateConstraint(Utilities::Settings::Optimizer::Constraint settings,
                           Model::Properties::VariablePropertyContainer *variables);

        // Constraint interface
        public:
            bool CaseSatisfiesConstraint(Case *c);
            void SnapCaseToConstraints(Case *c);

        private:
            double min_;
            double max_;
            QStringList affected_well_names_;
            QList<Model::Properties::ContinousProperty *> affected_real_variables_;
        };
    }
}

#endif //FIELDOPT_RATE_CONSTRAINT_H
