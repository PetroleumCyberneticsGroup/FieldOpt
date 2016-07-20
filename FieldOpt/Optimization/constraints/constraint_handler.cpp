#include "constraint_handler.h"
#include <iostream>

namespace Optimization {
    namespace Constraints {

        ConstraintHandler::ConstraintHandler(QList<Utilities::Settings::Optimizer::Constraint> constraints,
                                             Model::Properties::VariablePropertyContainer *variables,
                                             Reservoir::Grid::Grid *grid)
        {
            // todo: figure out how to use verbosity_level_ read in from json driver file
            // without changing number of input variables for ConstraintHandler function?
            // Maybe adding a copy of global setting parameter (verbosity_level_) to
            // optimizer_, simulator_ and model_ objects so this value can be passed around?
            int verbosity_level_ = 0;

            for (Utilities::Settings::Optimizer::Constraint constraint : constraints) {
                switch (constraint.type) {
                    case Utilities::Settings::Optimizer::ConstraintType::BHP:
                        if (verbosity_level_>2) std::cout << "... add BHP constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new BhpConstraint(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::Rate:
                        if (verbosity_level_>2) std::cout << "... add RATE constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new RateConstraint(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::WellSplineLength:
                        if (verbosity_level_>2) std::cout << "... add WellSplineLength constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new WellSplineLength(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::WellSplineInterwellDistance:
                        if (verbosity_level_>2) std::cout << "... add InterwellDistance constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new InterwellDistance(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::
                        CombinedWellSplineLengthInterwellDistance:
                        if (verbosity_level_>2) std::cout << "... add CombinedSplineLengthInterwellDistance constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new CombinedSplineLengthInterwellDistance(constraint, variables));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::
                        CombinedWellSplineLengthInterwellDistanceReservoirBoundary:
                        if (verbosity_level_>2) std::cout << "... add CombinedSplineLengthInterwellDistanceReservoirBoundary constraint (constraint_handler.cpp)" << std::endl;
                        constraints_.append(new CombinedSplineLengthInterwellDistanceReservoirBoundary
                                                    (constraint, variables, grid));
                        break;
                    case Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary:
                        constraints_.append(new ReservoirBoundary(constraint, variables, grid));
                        break;

#ifdef WITH_EXPERIMENTAL_CONSTRAINTS
                        // Cases for constraints in the experimental_constraints directory go here
#endif
                    default:
                        break;
                }
            }

#ifdef WITH_EXPERIMENTAL_CONSTRAINTS
    if (verbosity_level_>2) std::cout << "Using experimental constraints" << std::endl;
#else
    if (verbosity_level_>2) std::cout << "Not using experimental constraints" << std::endl;
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
