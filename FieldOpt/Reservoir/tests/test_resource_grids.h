#ifndef FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
#define FIELDOPT_TEST_RESOURCE_RESERVOIRS_H

#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Utilities/tests/test_resource_example_file_paths.hpp"

namespace TestResources {
    class TestResourceGrids {
    protected:
        TestResourceGrids() {
            grid_5spot_ = new Reservoir::Grid::ECLGrid(ExampleFilePaths::grid_5spot_);
            grid_horzwel_ = new Reservoir::Grid::ECLGrid(ExampleFilePaths::grid_horzwel_);
        }

        Reservoir::Grid::Grid *grid_5spot_;
        Reservoir::Grid::Grid *grid_horzwel_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
