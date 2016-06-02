#ifndef FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
#define FIELDOPT_TEST_RESOURCE_RESERVOIRS_H

#include "../reservoir/grid/grid.h"
#include "../reservoir/grid/eclgrid.h"
#include "Utilities/tests/test_resource_example_file_paths.h"

namespace TestResources {
    class TestResourceGrids {
    protected:
        TestResourceGrids() {
            grid_5spot_ = new Model::Reservoir::Grid::ECLGrid(ExampleFilePaths::grid_5spot_);
            grid_horzwel_ = new Model::Reservoir::Grid::ECLGrid(ExampleFilePaths::grid_horzwel_);
        }

        Model::Reservoir::Grid::Grid *grid_5spot_;
        Model::Reservoir::Grid::Grid *grid_horzwel_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
