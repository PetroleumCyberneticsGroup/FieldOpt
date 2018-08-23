/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Einar J.M. Baumann (2016) <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar J.M. Baumann (2017) <einar.baumann@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef WELLINDEXCALCULATOR_H
#define WELLINDEXCALCULATOR_H

#include <Eigen/Dense>
#include <vector>
#include <Eigen/Core>
#include "Reservoir/grid/grid.h"
#include "intersected_cell.h"

namespace Reservoir {
namespace WellIndexCalculation {

using namespace Eigen;
using namespace std;

class WellDefinition {
 public:
  string wellname;
  vector<Vector3d> heels;
  vector<Vector3d> toes;
  vector<double> heel_md;
  vector<double> toe_md;
  vector<double> well_length;
  vector<double> radii;
  vector<double> skins;

 public:
  static void ReadWellsFromFile(string file_path,
                                vector<WellDefinition>& wells);
};

/*!
 * \brief The WellIndexCalculation class deduces the well blocks
 * and their respective well indices/transmissibility factors for
 * one or more well splines defined by a heel and a toe.
 *
 * Note that some of the internal datastructures in this class seem
 * more complex than they need to be. This is because the internal
 * methods support well splines consisting of more than one point.
 * This is, however, not yet supported by the Model library and so
 * have been "hidden".
 *
 * Credit for computations in this class goes to @hilmarm.
 */
class WellIndexCalculator {
 public:
  WellIndexCalculator(){}
  WellIndexCalculator(Grid::Grid *grid);

  /*!
   * \brief Compute the well block indices for all wells
   * \param wells Vector of wells
   * \return A map containing for each well given my its name, the
   * list of cells intersected by the well. Each intersected cell
   * has stored the well connectivity information.
   */
  void ComputeWellBlocks(map<string, vector<IntersectedCell>> &well_indices,
                         vector<WellDefinition> &wells, int rank = 0);


 private:
  /*!
   * \brief The Well struct holds the information needed to compute
   * the well blocks and their respective well indices for a well
   * spline consisting of a heel and a toe.
   */

  Grid::Grid *grid_; //!< The grid used in the calculations.
  double smallest_grid_cell_dimension_; //!< Smallest dimension of any grid cell. Used to calculate step lengths.

//  /*!
//   * \brief This should compute the point of intersection of one directional line with a box defined by its corners - Needs to be farther tested
//   * \param
//   * \return
//   */
//  bool GetIntersection(double fDst1, double fDst2,
//                       Vector3d P1, Vector3d P2,
//                       Vector3d &Hit);

//  /*!
//   * \brief Needs to be farther tested
//   * \param
//   * \return
//   */
//  bool InBox(Vector3d Hit, Vector3d B1, Vector3d B2, const int Axis);

//  /*!
//   * \brief returns true if directional line (L1, L2) intersects with the box  defined by (B1, B2) - Needs to be farther tested
//   * \param
//   * \return returns intersection point in Hit
//   */
//  bool CheckLineBox(Vector3d B1, Vector3d B2,
//                    Vector3d L1, Vector3d L2,
//                    Vector3d &Hit);

  /*!
   * \brief Returns true if the line defined by L1,L2
   * lies completely outside the box defined by B1,B2
   */
  bool IsLineCompletelyOutsideBox(Vector3d B1, Vector3d B2,
                                  Vector3d L1, Vector3d L2 );

  /*!
   * @brief Find a new endpoint (heel/toe) for a well if necessary.
   * @param bb_cells Cellst to search through.
   * @param start_pt New start point.
   * @param end_point End point.
   * @param cell The first cell intersected by the segment.
   * @return Returns true if the operation was succesful; otherwise false.
   */
  bool findEndpoint(const vector<int> &bb_cells,
                    Vector3d &start_pt,
                    Vector3d end_point,
                    Grid::Cell &cell,
                    int rank = 0) const;

 public:
  /*!
   * \brief Given a reservoir with blocks and a line (start_point
   * to end_point), return global index of all blocks interesected
   * by the line, as well as the point where the line enters the
   * block
   *
   * ?? by the line and the points of intersection
   *
   * \param intersected_cells Vector in which to store the cells
   * \param start_point The start point of the well path.
   * \param end_point The end point of the well path.
   * \todo
   * \param ?????? grid The grid object containing blocks/cells.
   * \param bb_cells
   * \param
   * \param
   * \param
   * \param
   *
   * \return A pair containing global indices of intersected
   * cells and the points where it enters each cell (and thereby
   * leaves the previous cell) of the line segment inside each
   * cell.
   */
  void collect_intersected_cells(vector<IntersectedCell> &isc_cells,
                                 Vector3d start_pt, Vector3d end_pt,
                                 double wb_rad, double skin_fac,
                                 vector<int> bb_cells,
                                 double& bb_xi, double& bb_yi, double& bb_zi,
                                 double& bb_xf, double& bb_yf, double& bb_zf,
                                 int rank = 0);

  /*!
   * \brief Find the point where the line between the start_point
   * and end_point exits a cell.
   *
   * Takes as input an entry_point end_point which defines the well
   * path. Finds the two points on the path which intersects the
   * block faces and chooses the one that is not the entry point,
   * i.e. the exit point.
   *
   * \todo Find a better name for the exception_point and describe it better.
   *
   * \param cell The cell to find the well paths exit point in.
   * \param start_point The start point of the well path.
   * \param end_point The end point of the well path.
   * \param exception_point A specific point we don't
   * want the function to end up in.
   *
   * \return The point where the well path exits the cell.
   */
  Vector3d find_exit_point(vector<IntersectedCell> &cells, int cell_index,
                           Vector3d &start_point, Vector3d &end_point,
                           Vector3d &exception_point);

  /*!
   * \brief Compute the well index (aka. transmissibility factor)
   * for a (one) single cell/block by using the Projection Well
   * Method (Shu 2005).
   *
   * Assumption: The block is fairly regular,
   * i.e. corners are straight angles.
   *
   * \note Corner points of Cell(s) are always listed in the same
   * order and orientation. (see Grid::Cell for illustration).
   *
   * \param icell Well block to compute the WI in.
   * \return Well index for block/cell
  */
  void compute_well_index(vector<IntersectedCell> &cells,
                          int cell_index, int rank = 0);

  /*!
   * \brief Auxilary function for compute_well_index function
   *
   * \param Lx lenght of projection in first direction
   * \param dy size block second direction
   * \param dz size block third direction
   * \param ky permeability second direction
   * \param kz permeability second direction
   * \param wellbore_radius the radius of the segment
   * \param skin_factor associated skin to the segment
   *
   * \return directional well index
  */
  double dir_well_index(double Lx,
                        double dy, double dz, double ky, double kz,
                        double wellbore_radius, double skin_factor);

  /*!
   * \brief Auxilary function(2) for compute_well_index function
   *
   * \param dx size block second direction
   * \param dy size block third direction
   * \param kx permeability second direction
   * \param ky permeability second direction
   *
   * \return directional wellblock radius
   */
  double dir_wellblock_radius(double dx, double dy,
                              double kx, double ky);

  /*!
   * @brief Check whether adding a cell to the list will introduce a cycle.
   * @param cells The list of intersected cells to look through.
   * @param grdcell The new cell to check.
   * @return True if adding grdcell will introduce a cycle; otherwise false.
   */
  bool introduces_cycle(vector<IntersectedCell> cells, Grid::Cell grdcell);

  /*!
   * @brief Recover from a cycle by traversing through the previous cell,
   * finding a new exit point, and taking a last step into the next cell,
   * finding it and its entry point.
   * @param prev_cell The previous cell found (that didnt introduce a cycle).
   * @param next_cell The next cell (the one that introduces a cycle).
   * @param bb_cells
   * @param entry_pt Entry point for the new cell will be set here.
   * @param exit_pt Exit point for the new cell will be set here.
   * @param start_pt The start point (heel) of the well segment.
   * @param end_pt The end point (toe) of the well segment.
   * @param step The current step.
   * @param epsilon Step increase.
   * @param cycle_count
   */
  void recover_from_cycle(IntersectedCell &prev_cell,
                          Grid::Cell & next_cell,
                          vector<int> bb_cells,
                          Vector3d &entry_pt,
                          Vector3d &exit_pt,
                          Vector3d start_pt,
                          Vector3d end_pt,
                          double &step,
                          double epsilon,
                          int cycle_count);

  // WIC Debug
  int dbg_mode = 1; //!< On/off printing of debug messages

};

}
}

#endif // WELLINDEXCALCULATOR_H
