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
#ifndef BOXCONSTRAINT_H
#define BOXCONSTRAINT_H

// ---------------------------------------------------------
#include "constraint.h"
#include <QPair>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
/*!
 * \brief The BhpConstraint class is a simple max/min
 * constraint for BHP values for a well.
 */
class BhpConstraint : public Constraint
{
 public:
  // -------------------------------------------------------
  // This class' constructor should not be used directly.
  // The constructors of subclasses should be used.
  BhpConstraint(
      ::Settings::Optimizer::Constraint settings,
      ::Model::Properties::VariablePropertyContainer *variables);


  // -------------------------------------------------------
  string name() override { return "BhpConstraint"; }


 public:
  // -------------------------------------------------------
  // Constraint interface
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);
  bool IsBoundConstraint() const override;

  // -------------------------------------------------------
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;

 private:
  // -------------------------------------------------------
  double min_;
  double max_;
  QStringList affected_well_names_;
  QList<Model::Properties::ContinousProperty *>
      affected_real_variables_;
};

}
}

#endif // BOXCONSTRAINT_H
