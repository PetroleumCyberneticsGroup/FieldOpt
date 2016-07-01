#include "combined_spline_length_interwell_distance_reservoir_boundary.h"
#include <iostream>

namespace Optimization {
    namespace Constraints {

        CombinedSplineLengthInterwellDistanceReservoirBoundary
        ::CombinedSplineLengthInterwellDistanceReservoirBoundary(
                Utilities::Settings::Optimizer::Constraint settings,
                Model::Properties::VariablePropertyContainer *variables,
                Reservoir::Grid::Grid *grid)
        {
            max_iterations_ = settings.max_iterations;
            Utilities::Settings::Optimizer::Constraint dist_constr_settings;
            dist_constr_settings.wells = settings.wells;
            dist_constr_settings.min = settings.min_distance;
            distance_constraint_ = new InterwellDistance(dist_constr_settings, variables);
            std::cout << "Initialized distance constraint." << std::endl;

            length_constraints_ = QList<WellSplineLength *>();
            for (QString wname : settings.wells) {
                Utilities::Settings::Optimizer::Constraint len_constr_settings;
                len_constr_settings.well = wname;
                len_constr_settings.min = settings.min_length;
                len_constr_settings.max = settings.max_length;
                length_constraints_.append(new WellSplineLength(len_constr_settings, variables));
                std::cout << "Initialized length constraint." << std::endl;

                boundary_constraints_.append(new ReservoirBoundary(len_constr_settings, variables, nullptr));
                std::cout << "Initialized boundary constraint." << std::endl;
            }
        }

        bool CombinedSplineLengthInterwellDistanceReservoirBoundary
        ::CaseSatisfiesConstraint(Case *c)
        {
            if (!distance_constraint_->CaseSatisfiesConstraint(c))
                return false;
            for (WellSplineLength *wsl : length_constraints_) {
                if (!wsl->CaseSatisfiesConstraint(c))
                    return false;
            }
            for (ReservoirBoundary *rb : boundary_constraints_) {
                if (!rb->CaseSatisfiesConstraint(c))
                    return false;
            }
            return true;
        }

        void CombinedSplineLengthInterwellDistanceReservoirBoundary
        ::SnapCaseToConstraints(Case *c)
        {
            for (int i = 0; i < max_iterations_; ++i) {
                if (CaseSatisfiesConstraint(c)) {
                    return;
                }
                else {
                    distance_constraint_->SnapCaseToConstraints(c);
                    for (WellSplineLength *wsl : length_constraints_) {
                        wsl->SnapCaseToConstraints(c);
                    }
                    for (ReservoirBoundary *rb : boundary_constraints_) {
                        rb->SnapCaseToConstraints(c);
                    }
                }
            }
        }

    }
}
