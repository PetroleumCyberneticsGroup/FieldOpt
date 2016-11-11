#ifndef CONSTRAINTHANDLER_H
#define CONSTRAINTHANDLER_H
#include "constraint.h"
#include "bhp_constraint.h"
#include "well_spline_length.h"
#include "interwell_distance.h"
#include "combined_spline_length_interwell_distance.h"
#include "combined_spline_length_interwell_distance_reservoir_boundary.h"
#include "reservoir_boundary.h"
#include "rate_constraint.h"
#include "Optimization/case.h"
#include "Model/properties/variable_property_container.h"
#include "Settings/optimizer.h"
#include "Reservoir/grid/grid.h"

#include <QList>
#ifdef WITH_EXPERIMENTAL_CONSTRIANTS
// Includes for constraints in the experimental_constraints dir go here.
#endif

namespace Optimization {
    namespace Constraints {

        /*!
         * \brief The ConstraintHandler class facilitates the initialization and usage of multiple constraints.
         */
        class ConstraintHandler
        {
        public:
            ConstraintHandler(QList<Settings::Optimizer::Constraint> constraints,
                              Model::Properties::VariablePropertyContainer *variables,
                              Reservoir::Grid::Grid *grid);
            bool CaseSatisfiesConstraints(Case *c); //!< Check if a Case satisfies _all_ constraints.
            void SnapCaseToConstraints(Case *c); //!< Snap all variables to _all_ constraints.

            QList<Constraint *> constraints() const { return constraints_; }

        private:
            QList<Constraint *> constraints_;
        };

    }
}

#endif // CONSTRAINTHANDLER_H
