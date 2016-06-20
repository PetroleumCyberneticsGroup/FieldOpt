#include "wellindexcalculator.h"
#include "geometry_functions/geometryfunctions.h"

namespace WellIndexCalculator {

    WellIndexCalculator::WellIndexCalculator(Reservoir::Grid::Grid *grid, double wellbore_radius)
    {
        grid_ = grid;
        wellbore_radius_ = wellbore_radius;
    }

    QList<WellIndexCalculator::BlockData> WellIndexCalculator::GetBlocks(QList<Eigen::Vector3d> points)
    {
        if (points.length() != 2)
            throw std::runtime_error("Currently, only well splines consisting of two points (heel and toe) are supported.");
        auto spline_points = QList<Vector3d>({points.first(), points.last()});
        QList<Reservoir::WellIndexCalculation::IntersectedCell> data = GeometryFunctions::well_index_of_grid(grid_, spline_points, wellbore_radius_);
        QList<BlockData> block_data = QList<BlockData>();
        for (auto icell : data) {
            BlockData block;
            block.i = icell.ijk_index().i();
            block.j = icell.ijk_index().j();
            block.k = icell.ijk_index().k();
            block.well_index = icell.well_index();
            block_data.append(block);
        }
        return block_data;
    }

    WellIndexCalculator::WellIndexCalculator(Reservoir::Grid::Grid *grid) {
        grid_ = grid;
    }

    QList<WellIndexCalculator::BlockData> WellIndexCalculator::ComputeWellBlocks(Vector3d heel, Vector3d toe, double wellbore_radius) {
        current_well_.heel = heel;
        current_well_.toe = toe;
        current_well_.wellbore_radius = wellbore_radius;
        return QList<BlockData>();
    }


}
