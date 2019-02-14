/******************************************************************************
   Copyright (C) 2015-2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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

using namespace Model::Properties;

class PolarSpline : public WellSpline
{
 public:
  PolarSpline(::Settings::Model::Well well_settings,
             Properties::VariablePropertyContainer *variable_container,
             Reservoir::Grid::Grid *grid,
             Reservoir::WellIndexCalculation::wicalc_rixx *wic
  );

  virtual QList<WellBlock *> *GetWellBlocks() override;


 private:
  SplinePoint *midpoint_;
  Properties::ContinousProperty *azimuth_;
  Properties::ContinousProperty *length_;
  Properties::ContinousProperty *elevation_;

  /* @brief Compute the two other points defined by the midpoint, length and angles.
   * Sets the spline_point member.
   */
  void computePoints();

};

}
}
}

#endif // POLARSPLINE_H
