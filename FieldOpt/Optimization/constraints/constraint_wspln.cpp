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
#include "constraint_wspln.h"
#include <boost/lexical_cast.hpp>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
WellSplineConstraint::Well
WellSplineConstraint::initializeWell(
    QList<Model::Properties::ContinousProperty *> vars) {

  // -------------------------------------------------------
  if (vars.length() != 6)
    throw std::runtime_error(
        "Incorrect number of variables ("
            + boost::lexical_cast<std::string>(vars.length())
            + ")passed to the initialize well method.");

  // -------------------------------------------------------
  Well well;
  for (auto var : vars) {

    // -----------------------------------------------------
    // Should be {SplinePoint, WELLNAME, heel/toe, x/y/z}
    QStringList nmcomps = var->name().split("#");

    // -----------------------------------------------------
    if (QString::compare("heel", nmcomps[2]) == 0) {

      if (QString::compare("x", nmcomps[3]) == 0) {
        well.heel.x = var->id();

      } else if (QString::compare("y", nmcomps[3]) == 0) {
        well.heel.y = var->id();

      } else if (QString::compare("z", nmcomps[3]) == 0) {
        well.heel.z = var->id();

      } else {
        throw std::runtime_error(
            "Unable to parse variable "
                + var->name().toStdString());
      }

      // ---------------------------------------------------
    } else if (QString::compare("toe", nmcomps[2]) == 0) {

      if (QString::compare("x", nmcomps[3]) == 0) {
        well.toe.x = var->id();

      } else if (QString::compare("y", nmcomps[3]) == 0) {
        well.toe.y = var->id();

      } else if (QString::compare("z", nmcomps[3]) == 0) {
        well.toe.z = var->id();

      } else {
        throw std::runtime_error(
            "Unable to parse variable "
                + var->name().toStdString());
      }

      // ---------------------------------------------------
    } else {
      throw std::runtime_error(
          "Unable to parse variable "
              + var->name().toStdString());
    }
  }
  return well;
}

// =========================================================
QPair<Eigen::Vector3d, Eigen::Vector3d>
WellSplineConstraint::GetEndpointValueVectors(Case *c,
                                              Well well) {

  // -------------------------------------------------------
  double hx = c->real_variables()[well.heel.x];
  double hy = c->real_variables()[well.heel.y];
  double hz = c->real_variables()[well.heel.z];

  // -------------------------------------------------------
  double tx = c->real_variables()[well.toe.x];
  double ty = c->real_variables()[well.toe.y];
  double tz = c->real_variables()[well.toe.z];

  // -------------------------------------------------------
  Eigen::Vector3d heel(hx, hy, hz);
  Eigen::Vector3d toe(tx, ty, tz);

  // -------------------------------------------------------
  return qMakePair(heel, toe);
}


}
}
