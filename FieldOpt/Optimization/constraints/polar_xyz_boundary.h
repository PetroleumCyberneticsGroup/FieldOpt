/******************************************************************************
   Created by Brage on 31/03/19.
   Copyright (C) 2018 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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


#include "reservoir_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include "constraint.h"
#include "Reservoir/grid/grid.h"
#include "well_spline_constraint.h"

namespace Optimization {
namespace Constraints {

/*!
 * @brief This class implements the box-constraint with the PolarSpline well definition.
 * The purpose is to contain the midpoint within the specified box.
 *
 * Note: Works well with the 3d box intersection generator found in ResInsight
 * @ ResInsight.org
 */

class PolarXYZBoundary : public Constraint, WellSplineConstraint {
public:
    PolarXYZBoundary(const Settings::Optimizer::Constraint &settings,
                      Model::Properties::VariablePropertyContainer *variables,
                      Reservoir::Grid::Grid *grid);
    string name() override { return "PolarXYZBoundary"; }
    // Constraint interface
public:
    bool CaseSatisfiesConstraint(Case *c);
    void SnapCaseToConstraints(Case *c);
    bool IsBoundConstraint() const override { return true; }
    Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
    Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;


protected:
    double xmin_, xmax_, ymin_, ymax_, zmin_, zmax_; //!< Constraint limits for the box
    QList<int> index_list_; //!< Index list for the cells in the reservoir that are within the box
    Reservoir::Grid::Grid *grid_; //!< Grid, as defined in Reservoir/grid.h
    Well affected_well_; //!< The affected well

};
}
}
