/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by M.Bellout (2017) <mathias.bellout@ntnu.no, chakibbb@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>

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
 * @brief This file contains the main function for the stand-alone
 * well index calculator executable.
 */

#include "main.hpp"
#include "wellindexcalculator.h"
#include "boost/filesystem.hpp"
#include <Reservoir/grid/eclgrid.h>

using namespace Reservoir::WellIndexCalculation;
using namespace std;

int main(int argc, const char *argv[])
{
    // Initialize some variables from the runtime arguments
    Eigen::setNbThreads(1);
    // MB: introduced by OV for standalone library use;
    // check if really needed <- AGC not sure what this does

    auto vm = createVariablesMap(argc, argv);

    // Get the path to the grid file
    string gridpth = vm["grid"].as<string>();

    // Initialize the Grid and WellIndexCalculator objects
    Reservoir::Grid::ECLGrid* grid;
    try 
    {
        grid = new Reservoir::Grid::ECLGrid(gridpth);
    }
    catch (const std::runtime_error& e) 
    {
        cout << "Error reading the Eclipse grid " << e.what() << endl;
        cout << "The program will stop now";
        exit(EXIT_FAILURE);
    }
    
    auto wic = WellIndexCalculator(grid);
    vector<WellDefinition> wells;

    if (vm.count("well-filedef") == 1) 
    {
        assert(boost::filesystem::exists(vm["well-filedef"].as<string>()));
        WellDefinition::ReadWellsFromFile(vm["well-filedef"].as<string>(), wells);
    }
    else 
    {
        wells.push_back(WellDefinition());
        
        if (vm.count("well-name")) {
            wells.at(0).wellname = vm["well-name"].as<string>();
        }
        else
        {
        	wells.at(0).wellname = "unnamed_well";
        }
        
        wells.at(0).heels.push_back(Eigen::Vector3d(vm["heel"].as<vector<double>>().data()));
        wells.at(0).toes.push_back(Eigen::Vector3d(vm["toe"].as<vector<double>>().data()));
        wells.at(0).radii.push_back(vm["radius"].as<double>());
        wells.at(0).skins.push_back(vm["skin-factor"].as<double>());
    }
    
    // Compute the well blocks
    map<string, vector<IntersectedCell>> well_indices; 
    wic.ComputeWellBlocks(well_indices, wells);
    
    // Print as a COMPDAT table if the --compdat/-c flag was given
    if (vm.count("compdat")) 
    {
        printCompdat(well_indices);
    }
    // Otherwise, print as a CSV table
    else 
    {
        printCsv(well_indices);
    }

    if (vm.count("debug") > 0) {
        printDebug(well_indices);
    }

    exit(EXIT_SUCCESS);
}
