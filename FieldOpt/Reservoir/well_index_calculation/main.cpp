/// This file contains the main function for the stand-alone well index calculator executable.

#include "main.hpp"
#include "wellindexcalculator.h"
#include <grid/eclgrid.h>

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
