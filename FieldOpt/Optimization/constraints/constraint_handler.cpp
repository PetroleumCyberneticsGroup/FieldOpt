#include "constraint_handler.h"
#include <iostream>

namespace Optimization {
    namespace Constraints {

        ConstraintHandler::ConstraintHandler(QList<Utilities::Settings::Optimizer::Constraint> constraints, Model::Properties::VariablePropertyContainer *variables)
        {
            for (Utilities::Settings::Optimizer::Constraint constraint : constraints) {
                switch (constraint.type) {
                    case Utilities::Settings::Optimizer::ConstraintType::BHP:
                        constraints_.append(new BhpConstraint(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::Rate:
                        constraints_.append(new RateConstraint(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::WellSplineLength:
                        constraints_.append(new WellSplineLength(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::WellSplineInterwellDistance:
                        constraints_.append(new InterwellDistance(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::CombinedWellSplineLengthInterwellDistance:
                        constraints_.append(new CombinedSplineLengthInterwellDistance(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary:
                        constraints_.append(new ReservoirBoundary(constraint, variables, nullptr)); // TODO: Take grid as input
                        break;
#ifdef WITH_EXPERIMENTAL_CONSTRIANTS
                        // Cases for constraints in the experimental_constraints directory go here
#endif
                    default:
                        break;
                }
            }
#ifdef WITH_EXPERIMENTAL_CONSTRIANTS
            std::cout << "Using experimental constaints" << std::endl;
#else
            std::cout << "Not using experimental constaints" << std::endl;
#endif
        }

        bool ConstraintHandler::CaseSatisfiesConstraints(Case *c)
        {
            for (Constraint *constraint : constraints_) {
                if (!constraint->CaseSatisfiesConstraint(c)) {
                    return false;
                }
            }
            return true;
        }

        void ConstraintHandler::SnapCaseToConstraints(Case *c)
        {
            for (Constraint *constraint : constraints_) {
                constraint->SnapCaseToConstraints(c);
            }
        }

    }
}
