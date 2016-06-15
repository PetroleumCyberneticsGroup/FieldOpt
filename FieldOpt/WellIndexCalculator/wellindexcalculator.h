#ifndef WELLINDEXCALCULATOR_H
#define WELLINDEXCALCULATOR_H

#include <QList>
#include "Reservoir/grid/grid.h"

namespace WellIndexCalculator {

    class WellIndexCalculator
    {
    public:
        WellIndexCalculator(Reservoir::Grid::Grid *grid, double wellbore_radius);

        struct BlockData {
            int i;
            int j;
            int k;
            double well_index;
        };

        QList<BlockData> GetBlocks(QList<Eigen::Vector3d> points);

    private:
        Reservoir::Grid::Grid *grid_;
        double wellbore_radius_;
    };

}

#endif // WELLINDEXCALCULATOR_H
