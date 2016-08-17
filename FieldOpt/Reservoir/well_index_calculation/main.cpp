/// This file contains the main function for the stand-alone well index calculator executable.

#include "main.hpp"
#include "wellindexcalculator.h"
#include <grid/eclgrid.h>


int main(int argc, const char *argv[]) {
    // Initialize some variables from the runtime arguments
    auto vm = createVariablesMap(argc, argv);
    auto heel = Eigen::Vector3d(vm["heel"].as<std::vector<double>>().data());
    auto toe = Eigen::Vector3d(vm["toe"].as<std::vector<double>>().data());
    QString gridpth = QString::fromStdString(vm["grid"].as<std::string>());
    double wellbore_radius = vm["radius"].as<double>();

    // Initialize the Grid and WellIndexCalculator objects
    auto grid = new Reservoir::Grid::ECLGrid(gridpth);
    auto wic = WellIndexCalculator(grid);

    // Compute the well blocks
    auto well_blocks = wic.ComputeWellBlocks(heel, toe, wellbore_radius);

    if (vm.count("compdat")) { // Print as a COMPDAT table if the --compdat/-c flag was given
        QString well_name = QString::fromStdString(vm["well-name"].as<std::string>());
        printCompdat(well_blocks, well_name, wellbore_radius);
    }
    else { // Otherwise, print as a CSV table
        printCsv(well_blocks);
    }
    return 0;
}
