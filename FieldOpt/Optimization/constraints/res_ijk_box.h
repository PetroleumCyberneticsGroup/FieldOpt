/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef FIELDOPT_RESERVOIRBOUNDARY_H
#define FIELDOPT_RESERVOIRBOUNDARY_H

// ---------------------------------------------------------
#include "constraint.h"
#include "constraint_wspln.h"
#include "Reservoir/grid/grid.h"

//==========================================================
namespace Optimization {
namespace Constraints {

/*!
 * \brief The ReservoirBoundary class deals with
 * imposing and checking reservoir boundary constraints
 *
 *  The class takes a boundary as input in the form of
 *  i,j and k min,max, i.e. a 'box' in the grid i,j,k
 *  coordinates. It can check wether or not a single
 *  well is inside the given box domain and, if needed,
 *  project the well onto the domain.
 *
 *  \todo Figure out a more effective way to enforce
 *  the box constraints (TASK A), then figure out way
 *  boundary constraints for non-box (parallelogram)
 *  shapes (TASK B); finally, define this constraint
 *  (out of ReservoirBoundary) as a standalone
 *  constraint (call it Box) (TASK C)
 *
 *  Steps for (A):
 *  1. find edge cells of the box [x] + unit test [],
 *
 *  2. get corner points for each of the cells [] + unit test [],
 *
 *  3. find the corner points of the entire box (assuming the box is a
 *  parallelogram, which may not be true for the top and bottom planes
 *  -> figure out how to deal with this later) [] + unit test [],
 *
 *  4. print the box data to log for external visualization
 *
 *  5. figure out if the current point is inside or outside
 *  the box, e.g., create a BoxEnvelopsPoint function
 *
 *  6. if outside, project point onto nearest point on plane
 *
 *  Steps for (B):
 */
class ReservoirBoundary : public Constraint, WellSplineConstraint
{
 public:
  ReservoirBoundary(const Settings::Optimizer::Constraint &settings,
                    Model::Properties::VariablePropertyContainer *variables,
                    Reservoir::Grid::Grid *grid);
  string name() override { return "ReservoirBoundary"; }

  // Constraint interface
 public:
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);
  bool IsBoundConstraint() const override { return true; }

  /*!
   * @brief Initialize the normalizer parameters.
   *
   * For now just sets the parameters to the default value.
   *
   * \todo Properly implement this.
   *
   * @param cases Cases to be used when calculating the parameters.
   */
  void InitializeNormalizer(QList<Case *> cases) override;

  /*!
   * @brief Get the penalty for a case.
   *
   * For now, this returns INFINITY if the constraint is violated; otherwise
   * it returns 0.0. This corresponds to death penalty.
   *
   * \todo Propery implement this.
   * @param c The case to calculate the penalty for.
   * @return INFINITY if the constraint is violated; otherwise false.
   */
  double Penalty(Case *c) override;

  /*!
   * @brief Get the normalized penalty for a case.
   *
   * For now, this returns 1.0 if the constraint is violated; otherwise it returns
   * 0.0. This corresponds to death penalty.
   * \todo Properly implement this.
   * @param c The case to calculate the penalty for.
   * @return 1.0 if the constraint is violated; otherwise false.
   */
  long double PenaltyNormalized(Case *c) override;

  /*!
   * @brief Get the lower bounds. This will return the x, y and
   * z values for the _center_ of the cells in the corners of the
   * defined box.
   */
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;

  /* \brief Function getListOfBoxEdgeCellIndices uses the limits
   * defining the box constraint to find the cells that constitute
   * the edges of the box
   */
  QList<int> returnListOfBoxEdgeCellIndices() const { return index_list_edge_; }

  void findCornerCells();
//            QList<int> index_corner_cells_;

 private:
  int imin_, imax_, jmin_, jmax_, kmin_, kmax_;
  QList<int> index_list_;
  Reservoir::Grid::Grid *grid_;
  Well affected_well_;
  QList<int> getListOfCellIndices();

  QList<int> getIndicesOfEdgeCells();
  QList<int> index_list_edge_;

  void printCornerXYZ(std::string str_out, Eigen::Vector3d vector_xyz);
};
}
}

#endif //FIELDOPT_RESERVOIRBOUNDARY_H
