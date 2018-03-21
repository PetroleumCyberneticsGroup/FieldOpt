/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

// ---------------------------------------------------------
#ifndef INTERWELLDISTANCE_H
#define INTERWELLDISTANCE_H

// ---------------------------------------------------------
#include "constraint.h"
#include "well_spline_constraint.h"
#include <Eigen/Core>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
class InterwellDistance : public Constraint, WellSplineConstraint
{
 public:
  // -------------------------------------------------------
  InterwellDistance(Settings::Optimizer::Constraint settings,
                    Model::Properties::VariablePropertyContainer *variables);

  string name() override { return "InterwellDistance"; }

  // -------------------------------------------------------
  // Constraint interface
 public:
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);

  // -------------------------------------------------------
  /*!
   * @brief Set the normalizer parameters.
   *
   * - The max value (L) is set to 1.0.
   * - The steepness (k) is set to 1.0 divided by the smallest distance found between two endpoints.
   * - The midpoint (x_0) is set to the smallest distance found between two endpoints divided by 2.0.
   * @param cases Cases to be used to determine parameter values.
   */
  void InitializeNormalizer(QList<Case *> cases) override;
  double Penalty(Case *c) override;
  long double PenaltyNormalized(Case *c) override;

 private:
  // -------------------------------------------------------
  double distance_;
  QList<Well> affected_wells_;

  // -------------------------------------------------------
  //! Calculate the distances between the endpoints for two wells
  vector<double> endpointDistances(Case *c);

};

}
}

#endif // INTERWELLDISTANCE_H
