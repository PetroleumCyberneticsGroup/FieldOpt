/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>,
   Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#ifndef FIELDOPT_POLAR_WELL_LENGTH_H
#define FIELDOPT_POLAR_WELL_LENGTH_H

#include "Optimization/constraints/constraint.h"

namespace Optimization {
namespace Constraints {

class PolarWellLength : public Constraint {
 public:
  PolarWellLength(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables);
  bool CaseSatisfiesConstraint(Case *c) override;
  void SnapCaseToConstraints(Case *c) override;
  bool IsBoundConstraint() const override;
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;
  string name() override;

 public:
  double minimum_length_;
  double maximum_length_;
  QUuid affected_variable_;

};

}
}


#endif //FIELDOPT_POLAR_WELL_LENGTH_H
