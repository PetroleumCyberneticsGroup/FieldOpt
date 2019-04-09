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

#ifndef WELLSPLINE_H
#define WELLSPLINE_H

#include "trajectory.h"
#include "WellIndexCalculation/intersected_cell.h"
#include "WellIndexCalculation/wicalc_rixx.h"
#include "Reservoir/grid/eclgrid.h"
#include "Model/wells/wellbore/wellblock.h"
#include <QList>

namespace Model {
namespace Wells {
namespace Wellbore {

using namespace Model::Properties;

/*!
 * \brief The WellSpline class Generates the well blocks making up the trajectory from a set of spline points.
 * It uses the WellIndexCalculation library to do this.
 */
class WellSpline
{
 public:
  WellSpline(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid,
             Reservoir::WellIndexCalculation::wicalc_rixx *wic
  );
  WellSpline();

  /*!
   * \brief GetWellBlocks Get the set of well blocks with proper WI's defined by the spline.
   * \return
   */
  virtual QList<WellBlock *> *GetWellBlocks();
  int GetTimeSpentInWIC() const { return seconds_spent_in_compute_wellblocks_; }

  struct SplinePoint {
    ContinousProperty *x;
    ContinousProperty *y;
    ContinousProperty *z;
    Eigen::Vector3d ToEigenVector() const;
    void FromEigenVector(const Eigen::Vector3d vec);
  };

  bool HasGridChanged() const;
  bool HasSplineChanged() const;

  /*!
   * Get spline points (for debugging purposes).
   */
  QList<SplinePoint *> GetSplinePoints() const { return spline_points_; }


 protected:
  Reservoir::Grid::Grid *grid_;
  Reservoir::WellIndexCalculation::wicalc_rixx *wic_;
  Settings::Model::Well well_settings_;
  int seconds_spent_in_compute_wellblocks_; //!< Number of seconds spent in the ComputeWellBlocks() method.
  bool is_variable_;
  bool use_bezier_spline_;

  /*!
   * @brief Get compute the well blocks to be returned by GetWellBlocks.
   * @return
   */
  QList<WellBlock *> *computeWellBlocks();

  virtual /*!
   * @brief Get the vector of spline points to be used. Will return converted spline_points if not using
   * bezier method; else will call convertToBezierSpline().
   * @return
   */
  vector<Eigen::Vector3d> getPoints() const;

  /*!
   * @brief Use the current set of spline_points_ to generate a set of points representing a
   * bezier curve formed from the original points.
   */
  vector<Eigen::Vector3d> convertToBezierSpline() const;

  QList<SplinePoint *> spline_points_;

  std::string last_computed_grid_; //!< Contains the path to the last grid used by WIC.
  std::vector<Eigen::Vector3d> last_computed_spline_; //!< Contains the last spline points used by WIC. Used to determine if the spline has changed.

  /*!
   * \brief getWellBlock Convert the BlockData returned by the WIC to a WellBlock with a Perforation.
   * \note The IJK indexes are incremented by on to account for the zero-inclusive indices used by
   * the ERT library. This is necessary because ECL and ADGPRS both use zero-exclusive indices.
   * \param block_data
   * \return
   */
  WellBlock *getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data);

  std::vector<Eigen::Vector3d> create_spline_point_vector() const;

  void spline_points_from_import(Settings::Model::Well &well_settings);


  /*!
   * List containing imported well blocks. This will be used if
   *    1. A trajectory has been imported for the well, _and_
   *    2. The well trajectory is not variable.
   *
   * This list will converted to a list of IntersectedCell objects
   * to be passed to the well index calculator, which
   * will _only_ calculate the well indices. This saves the time
   * consuming step of re-computing the intersection point, which
   * is a particularly big problem when working with multiple
   * realizations.
   *
   * Note that the conversion to IntersectedCell objects will only
   * happen when GetWellBlocks is called. This is to account for the
   * fact that the grid_ object may change throughout the run.
   */
  std::vector<Settings::TrajectoryImporter::ImportedWellBlock> imported_wellblocks_;

  std::vector<Reservoir::WellIndexCalculation::IntersectedCell> convertImportedWellblocksToIntersectedCells();
};

}
}
}

#endif // WELLSPLINE_H
