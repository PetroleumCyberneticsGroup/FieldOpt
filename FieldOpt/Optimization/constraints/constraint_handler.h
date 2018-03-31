/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

// -----------------------------------------------------------------
#ifndef CONSTRAINTHANDLER_H
#define CONSTRAINTHANDLER_H

// -----------------------------------------------------------------
// FIELDOPT::CONSTRAINTS
#include "constraint.h"
#include "bhp_constraint.h"
#include "well_spline_length.h"
#include "interwell_distance.h"
#include "combined_spline_length_interwell_distance.h"
#include "combined_spline_length_interwell_distance_reservoir_boundary.h"
#include "reservoir_boundary.h"
#include "pseudo_cont_boundary_2d.h"
#include "rate_constraint.h"
#include "iwd_constraint.h"

// -----------------------------------------------------------------
// FIELDOPT::OPTIMIZATION/MODEL/SETTINGS/RESERVOIR
#include "Optimization/case.h"
#include "Model/properties/variable_property_container.h"
#include "Settings/optimizer.h"
#include "Reservoir/grid/grid.h"
//#include <Runner/logger.h>

// -----------------------------------------------------------------
// Qt
#include <QList>


#ifdef WITH_EXPERIMENTAL_CONSTRIANTS
// Includes for constraints in the experimental_constraints dir go here.
#endif

// -----------------------------------------------------------------
namespace Optimization {
namespace Constraints {

// -----------------------------------------------------------------
/*!
 * \brief The ConstraintHandler class facilitates the
 * initialization and usage of multiple constraints.
 */
class ConstraintHandler
{
 public:

  // -----------------------------------------------------------------
  ConstraintHandler(
      QList<Settings::Optimizer::Constraint> constraints,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid,
      Settings::Optimizer *settings);

  // -----------------------------------------------------------------
  // Check if a Case satisfies _all_ constraints.
  bool CaseSatisfiesConstraints(Case *c);

  // -----------------------------------------------------------------
  // Snap all variables to _all_ constraints.
  void SnapCaseToConstraints(Case *c);

  // -----------------------------------------------------------------
  QList<Constraint *> constraints() const { return constraints_; }

  // -----------------------------------------------------------------
  /*!
   * @brief Check whether any of the constraints within are boundary constraints.
   */
  bool HasBoundaryConstraints() const;

  // -----------------------------------------------------------------
  /*!
   * @brief Initialize the normalizers for all constraints.
   * @param cases Cases to be used for determining parameters.
   */
  void InitializeNormalizers(QList<Case *> cases);

  // -----------------------------------------------------------------
  /*!
   * @brief Get the sum of all normalized penalties multiplied by their respective weights.
   * @param c The case to get the penalties for.
   * @return Weighted sum of all normalized penalties
   */
  long double GetWeightedNormalizedPenalties(Case *c);

  // -----------------------------------------------------------------
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const;

 private:
  QList<Constraint *> constraints_;

};

}
}

#endif // CONSTRAINTHANDLER_H
