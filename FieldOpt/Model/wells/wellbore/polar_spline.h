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
#ifndef POLARSPLINE_H
#define POLARSPLINE_H

#include "wellspline.h"

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * This class defines a well parameterization based on polar/spherical coordinates.
 * The well is defined by a midpoint, along with a well length, azimuth and elevation.
 *
 *   - The midpoint specifies the middle of the well. The remaining parameters are used to create
 *     two additional equdistant points on either side of the midpoint.
 *   - Elevation specifies the angle of the well relative to the z plane. The angle should be given
 *     in radians. 90 degrees is completely horizontal, while 0 degrees is completely vertical.
 *   - Azimuth is the rotation in the x-y plane (in degrees). 0 degrees is parallel to the x axis
 *     while 90 degrees is parallel to the y axis.
 *   - Length defines the length of the well in meters. The endpoints will be length/2 out on either
 *     side of the midpoint.
 */
class PolarSpline : public WellSpline
{
 public:
  PolarSpline(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid,
             Reservoir::WellIndexCalculation::wicalc_rixx *wic
  );

  /*!
   * @brief This function activates the WellIndexCalculator (WIC) and returns the blocks between the two
   * Spline Points as defined by the midpoint, azimuth, elevation and length.
   * @return
   */
  virtual QList<WellBlock *> *GetWellBlocks() override;


 private:
  SplinePoint *midpoint_; //!< The midpoint defined by x, y, and z in cartesian coordinates
  Properties::ContinousProperty *azimuth_; //!< The azimuth rotation in the x-y plane, with 0 degrees being parallel to the x axis
  Properties::ContinousProperty *length_; //!< The length of the well in meters, length/2 is used as the radius
  Properties::ContinousProperty *elevation_; //!< The angle of the well relative to the z plane

  /*!
   * @brief Compute the two other points defined by the midpoint, length and angles.
   * Sets the spline_point member.
   */

  void computePoints();

};

}
}
}

#endif // POLARSPLINE_H
