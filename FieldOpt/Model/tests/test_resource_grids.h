#ifndef FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
#define FIELDOPT_TEST_RESOURCE_RESERVOIRS_H

#include "../reservoir/grid/grid.h"
#include "../reservoir/grid/eclgrid.h"

class TestResourceGrids {
protected:
    TestResourceGrids(){
        grid_5spot_ = new Model::Reservoir::Grid::ECLGrid(file_path_5spot_);
        grid_horzwel_ = new Model::Reservoir::Grid::ECLGrid(file_path_horzwel_);
    }
    Model::Reservoir::Grid::Grid *grid_5spot_;
    Model::Reservoir::Grid::Grid *grid_horzwel_;

    QString file_path_horzwel_ = "../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";
    QString file_path_5spot_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
};

#endif //FIELDOPT_TEST_RESOURCE_RESERVOIRS_H
