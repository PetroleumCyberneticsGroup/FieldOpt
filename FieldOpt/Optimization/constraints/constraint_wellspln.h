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
#ifndef WELLSPLINECONSTRAINT_H
#define WELLSPLINECONSTRAINT_H

// ---------------------------------------------------------
#include "Settings/optimizer.h"
#include "Model/properties/variable_property_container.h"
#include <QPair>
#include <Eigen/Core>
#include "Optimization/case.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
/*!
 * \brief The WellSplineConstraint class acts as a parent
 * class for constraints dealing with well splines.
 *
 * This class provides datastructures with initialization
 * methods that ease the application of well spline
 * constraints.
 */
class WellSplineConstraint
{
 protected:
  WellSplineConstraint() {}

  // ---------------------------------------------------------
  /*!
   * \brief Coord struct holds the UUIDs for the variables
   * containing coordinate values of a WellSpline point.
   */
  struct Coord {
    QUuid x;
    QUuid y;
    QUuid z;
  };

  // ---------------------------------------------------------
  /*!
   * \brief Well struct holds the heel and toe coordinates
   * for a well defined by a WellSpline.
   */
  struct Well {
    Coord heel;
    Coord toe;
  };

  // ---------------------------------------------------------
  /*!
   * \brief initializeWell Initialize the Well and Coord
   * datastructures using the provided variables.
   * \param vars the six variables defining the wellspline.
   */
  Well initializeWell(
      QList<Model::Properties::ContinousProperty *> vars);

  // ---------------------------------------------------------
  /*!
   * @brief Get the vectors containing the endpoint
   * (heel/toe) coordinate values for a case.
   *
   * @param c The case to get the values from.
   * @param well The well to get the values for.
   *
   * @return A pair (heel, toe) of Eigen
   * vectors: [(hx, hy, hz), (tz, ty, tz)]
   */
  QPair<Eigen::Vector3d, Eigen::Vector3d>
  GetEndpointValueVectors(Case *c, Well well);

};

}
}

#endif // WELLSPLINECONSTRAINT_H
