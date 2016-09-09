/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

/*!
 * @brief This file contains the main function for the stand-alone well index calculator executable.
 */


#include "main.hpp"
#include "wellindexcalculator.h"
#include <Reservoir/grid/eclgrid.h>

using namespace std;

int main(int argc, const char *argv[]) {
    // Initialize some variables from the runtime arguments
    auto vm = createVariablesMap(argc, argv);
    auto heel = Eigen::Vector3d(vm["heel"].as<vector<double>>().data());
    auto toe = Eigen::Vector3d(vm["toe"].as<vector<double>>().data());
    string gridpth = vm["grid"].as<string>();
    double wellbore_radius = vm["radius"].as<double>();

    // Initialize the Grid and WellIndexCalculator objects
    auto grid = new Reservoir::Grid::ECLGrid(gridpth);
    auto wic = WellIndexCalculator(grid);

    // Compute the well blocks
    auto well_blocks = wic.ComputeWellBlocks(heel, toe, wellbore_radius);

    if (vm.count("compdat")) { // Print as a COMPDAT table if the --compdat/-c flag was given
        string well_name = vm["well-name"].as<string>();
        printCompdat(well_blocks, well_name, wellbore_radius);
    }
    else { // Otherwise, print as a CSV table
        printCsv(well_blocks);
    }
    return 0;
}
