#ifndef FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
#define FIELDOPT_TEST_RESOURCE_RESERVOIRS_H

#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"

namespace TestResources {
    class TestResourceGrids {
    protected:
        TestResourceGrids() {
            grid_horzwel_ = new Reservoir::Grid::ECLGrid("../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID");
            grid_5spot_ = new Reservoir::Grid::ECLGrid("../examples/ADGPRS/5spot/ECL_5SPOT.EGRID");
        }

        Reservoir::Grid::Grid *grid_5spot_;
        Reservoir::Grid::Grid *grid_horzwel_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
