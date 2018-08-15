/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef FIELDOPT_PACKER_CONSTRAINT_H
#define FIELDOPT_PACKER_CONSTRAINT_H

#include "constraint.h"

namespace Optimization {
namespace Constraints {

class PackerConstraint : public Constraint {
 public:
  PackerConstraint(Settings::Optimizer::Constraint settings, Model::Properties::VariablePropertyContainer *variables);
  string name() override;
  bool CaseSatisfiesConstraint(Case *c) override;
  void SnapCaseToConstraints(Case *c) override;
  bool IsBoundConstraint() const override;
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;

 private:
  double min_, max_; // Min/Max measured depth for packers (correspond to MD for the heel and toe of the well).
  QList<QUuid> affected_variables_;

};

}
}

#endif //FIELDOPT_PACKER_CONSTRAINT_H
