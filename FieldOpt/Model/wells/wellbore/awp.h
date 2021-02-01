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

#ifndef AWP_H
#define AWP_H

#include "trajectory.h"
#include "WellIndexCalculation/intersected_cell.h"
#include "WellIndexCalculation/wicalc_rixx.h"
#include "Reservoir/grid/eclgrid.h"
#include "Model/wells/wellbore/wellblock.h"
#include <QList>
#include "Settings/settings_exceptions.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

namespace Model {
namespace Wells {
namespace Wellbore {

using namespace Model::Properties;

/*!
* \brief The AWP class Generates the well blocks making up the trajectory from a set of spline points.
* It uses the WellIndexCalculation library to do this.
*/
class AWP : public WellSpline
{
public:
    AWP(::Settings::Model::Well well_settings,
                Properties::VariablePropertyContainer *variable_container,
                Reservoir::Grid::Grid *grid,
                Reservoir::WellIndexCalculation::wicalc_rixx *wic
    );

    /*!
     * @brief This function activates the WellIndexCalculator (WIC) and returns the blocks between the two
     * Spline Points as defined by the midpoint, azimuth, elevation and length.
     * @return
     */
    QList<WellBlock *> *computeWellBlocks() override;
    QList<WellBlock *> *GetWellBlocks() override;
    vector<Eigen::Vector3d> getPoints() const override;

    vector<Eigen::Vector3d> createAWPTrajectory() const;

private:
    HorizontalPoint *heel_; //!< Defined the heels in two cartesian coordinates
    HorizontalPoint *toe_; //!< Defined the toe in two cartesian coordinates

};

}
}
}

#endif // WELLSPLINE_H
