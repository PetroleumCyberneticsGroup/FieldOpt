/******************************************************************************
   Created by einar on 8/16/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_PSEUDOCONTBOUNDARY2D_H
#define FIELDOPT_PSEUDOCONTBOUNDARY2D_H

#include "constraint.h"
#include "Reservoir/grid/grid.h"

using namespace std;

namespace Optimization {
namespace Constraints {

/*!
 * @brief The PseudoContBoundary2D constraint must be used alongside the pseudo-continuous
 * vertical well variable type. It only works with flat reservoirs.
 */
class PseudoContBoundary2D : public Constraint {
 public:
  bool IsBoundConstraint() const override;
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;
  string name() override;
  void InitializeNormalizer(QList<Case *> cases) override;
  double Penalty(Case *c) override;
  long double PenaltyNormalized(Case *c) override;
  PseudoContBoundary2D(const Settings::Optimizer::Constraint &settings,
                       Model::Properties::VariablePropertyContainer *variables,
                       Reservoir::Grid::Grid *grid);
  bool CaseSatisfiesConstraint(Case *c) override;
  void SnapCaseToConstraints(Case *c) override;

 private:
  QString well_name_;
  int i_min_, i_max_, j_min_, j_max_;
  double x_min_, x_max_, y_min_, y_max_;
  QUuid affected_x_var_id_;
  QUuid affected_y_var_id_;
};

}
}

#endif //FIELDOPT_PSEUDOCONTBOUNDARY2D_H
