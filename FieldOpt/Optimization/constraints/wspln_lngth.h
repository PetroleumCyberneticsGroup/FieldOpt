/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef WELLSPLINELENGTH_H
#define WELLSPLINELENGTH_H

// ---------------------------------------------------------
#include "constraint.h"
#include "constraint_wspln.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
/*!
 * \brief The WellSplineLength class defines a constraint
 * on the maximum and minimum length of a well defined by
 * a WellSpline. It uses the WellIndexCalculation library.
 */
class WellSplineLength : public Constraint, WellSplineConstraint
{
 public:
  // -------------------------------------------------------
  WellSplineLength(
      ::Settings::Optimizer::Constraint &settings,
      ::Model::Properties::VariablePropertyContainer *variables);
  string name() override { return "WellSplineLength"; };

 public:
  // -------------------------------------------------------
  // Constraint interface
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);

  // -------------------------------------------------------
  void InitializeNormalizer(QList<Case *> cases) override;
  double Penalty(Case *c) override;
  long double PenaltyNormalized(Case *c) override;

  // -------------------------------------------------------
 private:
  double min_length_;
  double max_length_;
  Well affected_well_;

};

}
}

#endif // WELLSPLINELENGTH_H
