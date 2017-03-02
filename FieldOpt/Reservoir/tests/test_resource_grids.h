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

#ifndef FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
#define FIELDOPT_TEST_RESOURCE_RESERVOIRS_H

#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"

namespace TestResources {
    class TestResourceGrids {
    protected:
        TestResourceGrids() {
            grid_horzwel_ = new Reservoir::Grid::ECLGrid(
                "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID");
            grid_5spot_ = new Reservoir::Grid::ECLGrid(
                "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID");
        }

        Reservoir::Grid::Grid *grid_5spot_;
        Reservoir::Grid::Grid *grid_horzwel_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
