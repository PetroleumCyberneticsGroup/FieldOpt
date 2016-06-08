#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Optimization/case.h"
#include "Utilities/settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization {
    namespace Constraints {

/*!
 * \brief The Constraint class is the abstract parent class to all other constraint classes. One Constraint
 * object should be created for each defined constraint.
 */
        class Constraint
        {
        public:
            /*!
             * \brief CaseSatisfiesConstraint checks whether a case satisfies the constraints for all
             * applicable variables.
             * \param c The case to be checked.
             * \return True if the constraint is satisfied; otherwise false.
             */
            virtual bool CaseSatisfiesConstraint(Case *c) = 0;

            /*!
             * \brief SnapCaseToConstraints Snaps all variable values in the case to the closest value
             * that satisfies the constraint.
             * \param c The case that should have it's variable values snapped.
             */
            virtual void SnapCaseToConstraints(Case *c) = 0;
        };

    }
}

#endif // CONSTRAINT_H
