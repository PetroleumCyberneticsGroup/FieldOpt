#ifndef BOXCONSTRAINT_H
#define BOXCONSTRAINT_H

#include "constraint.h"
#include <QPair>

namespace Optimization {
    namespace Constraints {

/*!
 * \brief The BhpConstraint class Represents a is a simple max/min constraint for BHP
 * values for a well.
 */
        class BhpConstraint : public Constraint
        {
        public:
            BhpConstraint(::Utilities::Settings::Optimizer::Constraint settings, ::Model::Properties::VariablePropertyContainer *variables); //!< This class' constructor should not be used directly. The constructors of subclasses should be used.

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

#endif // BOXCONSTRAINT_H
