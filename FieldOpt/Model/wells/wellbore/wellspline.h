/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 23.09.2015 2015 by einar

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
#ifndef WELLSPLINE_H
#define WELLSPLINE_H

// ---------------------------------------------------------
#include "trajectory.h"
#include "Reservoir/grid/eclgrid.h"
#include "Model/wells/wellbore/wellblock.h"
#include "FieldOpt-WellIndexCalculator/wellindexcalculator.h"
#include <QList>

// ---------------------------------------------------------
#include "FieldOpt-WellIndexCalculator/resinxx/well_path.h"

// ---------------------------------------------------------
namespace Model {
namespace Wells {
namespace Wellbore {

// ---------------------------------------------------------
/*!
 * \brief The WellSpline class Generates the well blocks making
 * up the trajectory from a set of spline points. It uses the
 * WellIndexCalculation library to do this.
 */
class WellSpline
{
 public:
  // -------------------------------------------------------
  WellSpline(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid,
             RICaseData *ricasedata);

  // -------------------------------------------------------
  /*!
   * \brief GetWellBlocks Get the set of well blocks with
   * proper WI's defined by the spline.
   * \return
   */
  QList<WellBlock *> *GetWellBlocks(int rank=0);
  int GetTimeSpentInWIC() const
  { return time_cwb_wic_pcg_; }

  // -------------------------------------------------------
  double GetSplineDx() { return toe_x_->value() - heel_x_->value(); };
  double GetSplineDy() { return toe_y_->value() - heel_y_->value(); };
  double GetSplineDz() { return toe_z_->value() - heel_z_->value(); };
  double GetSplineZe() { return toe_z_->value(); };

 private:
  // -------------------------------------------------------
  Reservoir::Grid::Grid *grid_;
  Settings::Model::Well well_settings_;

  // -------------------------------------------------------
  RICaseData* ricasedata_;
  // RIReaderECL rireaderecl_;
  // RIGrid* rigrid_;
  // cvf::ref<RICaseData> ricasedatap_;

  // -------------------------------------------------------
  //!< # of seconds spent in the ComputeWellBlocks() method.
  int time_cwb_wic_pcg_;
  // int time_cwb_wic_rins_;
  // int time_cwb_wic_rinx_;
  int time_cwb_wic_rixx_;

  // -------------------------------------------------------
  Model::Properties::ContinousProperty *heel_x_;
  Model::Properties::ContinousProperty *heel_y_;
  Model::Properties::ContinousProperty *heel_z_;
  Model::Properties::ContinousProperty *toe_x_;
  Model::Properties::ContinousProperty *toe_y_;
  Model::Properties::ContinousProperty *toe_z_;

  // -------------------------------------------------------
  /*!
   * \brief getWellBlock Convert the BlockData returned by WIC
   * to a WellBlock with a Perforation.
   *
   * \note The IJK indexes are incremented by one to account for
   * the zero-inclusive indices used by the ERT library. This is
   * necessary because ECL and ADGPRS both use zero-exclusive
   * indices.
   *
   * \param block_data
   *
   * \return
   */
  WellBlock *getWellBlock(
      Reservoir::WellIndexCalculation::IntersectedCell block_data);
};

}
}
}

#endif // WELLSPLINE_H
