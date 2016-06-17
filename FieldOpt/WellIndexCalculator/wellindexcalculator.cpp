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
    QList<Eigen::Vector3d> startvec = {points.first()};
    QList<Eigen::Vector3d> endvec = {points.last()};
    QPair<QList<int>, QList<double>> data = GeometryFunctions::well_index_of_grid(grid_, startvec, endvec, wellbore_radius_);

    QList<BlockData> block_data = QList<BlockData>();
    for (int i = 0; i < data.first.size(); ++i) {
        BlockData block;
        int global_index = data.first[i];
        Reservoir::Grid::IJKCoordinate ijk = grid_->GetCell(global_index).ijk_index();
        block.i = ijk.i();
        block.j = ijk.j();
        block.k = ijk.k();
        block.well_index = data.second[i];
        block_data.append(block);
    }
    return block_data;
}

    WellIndexCalculator::WellIndexCalculator(Reservoir::Grid::Grid *grid) {
        grid_ = grid;
    }

    QList<BlockData> WellIndexCalculator::ComputeWellBlocks(Vector3d heel, Vector3d toe, double wellbore_radius) {
        current_well_.heel = heel;
        current_well_.toe = toe;
        current_well_.wellbore_radius = wellbore_radius;
        return QList<BlockData>();
    }


}
