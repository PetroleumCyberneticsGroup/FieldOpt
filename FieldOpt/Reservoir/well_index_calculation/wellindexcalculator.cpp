#include <iostream>
#include "wellindexcalculator.h"

namespace Reservoir {
    namespace WellIndexCalculation {

        WellIndexCalculator::WellIndexCalculator(Grid::Grid *grid, double wellbore_radius) {
            grid_ = grid;
            wellbore_radius_ = wellbore_radius;
        }

        QList<WellIndexCalculator::BlockData> WellIndexCalculator::GetBlocks(QList<Eigen::Vector3d> points) {
            if (points.length() != 2)
                throw std::runtime_error(
                        "Currently, only well splines consisting of two points (heel and toe) are supported.");
            heel_ = points.first();
            toe_ = points.last();
            auto spline_points = QList<Vector3d>({points.first(), points.last()});
            QList<IntersectedCell> data = compute_well_indices();
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

        WellIndexCalculator::WellIndexCalculator(Grid::Grid *grid) {
            grid_ = grid;
        }

        QList<WellIndexCalculator::BlockData> WellIndexCalculator::ComputeWellBlocks(Vector3d heel, Vector3d toe,
                                                                                     double wellbore_radius) {
            heel_ = heel;
            toe_ = toe;
            wellbore_radius_ = wellbore_radius;
            return QList<BlockData>();
        }

        QList<IntersectedCell> WellIndexCalculator::cells_intersected() {
            Grid::Cell last_cell = grid_->GetCellEnvelopingPoint(toe_);
            QList<IntersectedCell> intersected_cells;
            intersected_cells.append(IntersectedCell(grid_->GetCellEnvelopingPoint(heel_)));
            intersected_cells.first().set_entry_point(heel_);

            // If the first and last blocks are the same, return the block and start+end points
            if (last_cell.global_index() == intersected_cells.first().global_index()) {
                intersected_cells.first().set_exit_point(toe_);
                return intersected_cells;
            }

            // Make sure we follow line in the correct direction. (i.e. dot product positive)
            Vector3d exit_point = find_exit_point(intersected_cells.first(), heel_, toe_, heel_);
            if ((toe_ - heel_).dot(exit_point - heel_) <= 0.0) {
                exit_point = find_exit_point(intersected_cells.first(), heel_, toe_, exit_point);
            }
            intersected_cells.first().set_exit_point(exit_point);

            double epsilon = 0.01 / (toe_ - exit_point).norm();

            // Add previous exit point to list, find next exit point and all other up to the end_point
            while (true) {
                // Move into the next cell, add it to the list and set the entry point
                Vector3d move_exit_epsilon = exit_point * (1 - epsilon) + toe_ * epsilon;
                intersected_cells.append(IntersectedCell(grid_->GetCellEnvelopingPoint(move_exit_epsilon)));
                intersected_cells.last().set_entry_point(exit_point); // The entry point of each cell is the exit point of the previous cell

                // Terminate if we're in the last cell
                if (intersected_cells.last().global_index() == last_cell.global_index()) {
                    intersected_cells.last().set_exit_point(toe_);
                    break;
                }

                // Find the exit point of the cell and set it in the list
                exit_point = find_exit_point(intersected_cells.last(), exit_point, toe_, exit_point);
                intersected_cells.last().set_exit_point(exit_point);
                assert(intersected_cells.length() < 500);
            }
            assert(intersected_cells.last().global_index() == last_cell.global_index());
            return intersected_cells;
        }

        Vector3d WellIndexCalculator::find_exit_point(Grid::Cell &cell, Vector3d &entry_point,
                                                      Vector3d &end_point, Vector3d &exception_point) {
            Vector3d line = end_point - entry_point;

            // Loop through the cell faces untill we find one that the line intersects
            for (Grid::Cell::Face face : cell.faces()) {
                if (face.normal_vector.dot(line) != 0) { // Check that the line and face are not parallel.
                    auto intersect_point = face.intersection_with_line(entry_point, end_point);

                    // Check that the intersect point is on the correct side of all faces (i.e. inside the cell)
                    bool feasible_point = true;
                    for (auto p : cell.faces()) {
                        if (!p.point_on_same_side(intersect_point, 10e-6)) {
                            feasible_point = false;
                            break;
                        }
                    }

                    // Return the point if it is deemed feasible, not identical to the entry point, and going in the correct direction.
                    if (feasible_point && (exception_point - intersect_point).norm() > 10e-10
                        && (end_point - entry_point).dot(end_point - intersect_point) >= 0) {
                        return intersect_point;
                    }
                }
            }
            // If all fails, the line intersects the cell in a single point (corner or edge) -> return entry_point
            return entry_point;
        }

        double WellIndexCalculator::well_index_cell(IntersectedCell &icell) {
            double Lx = 0;
            double Ly = 0;
            double Lz = 0;

            for (int ii = 0; ii < icell.points().length() - 1; ++ii) { // Current segment ii
                // Compute vector from segment
                Vector3d current_vec = icell.points().at(ii+1) - icell.points().at(ii);

                /* Proejcts segment vector to directional spanning vectors and determines the length.
                 * of the projections. Note that we only only care about the length of the projection,
                 * not the spatial position. Also adds the lengths of previous segments in case there
                 * is more than one segment within the well.
                 */
                Lx = Lx + (icell.xvec() * icell.xvec().dot(current_vec) / icell.xvec().dot(icell.xvec())).norm();
                Ly = Ly + (icell.yvec() * icell.yvec().dot(current_vec) / icell.yvec().dot(icell.yvec())).norm();
                Lz = Lz + (icell.zvec() * icell.zvec().dot(current_vec) / icell.zvec().dot(icell.zvec())).norm();
            }

            // Compute Well Index from formula provided by Shu
            double well_index_x = (dir_well_index(Lx, icell.dy(), icell.dz(), icell.permy(), icell.permz()));
            double well_index_y = (dir_well_index(Ly, icell.dx(), icell.dz(), icell.permx(), icell.permz()));
            double well_index_z = (dir_well_index(Lz, icell.dx(), icell.dy(), icell.permx(), icell.permy()));
            return sqrt(well_index_x * well_index_x + well_index_y * well_index_y + well_index_z * well_index_z);
        }

        double WellIndexCalculator::dir_well_index(double Lx, double dy, double dz, double ky, double kz) {
            double silly_eclipse_factor = 0.008527;
            double well_index_i = silly_eclipse_factor * (2 * M_PI * sqrt(ky * kz) * Lx) /
                                  (log(dir_wellblock_radius(dy, dz, ky, kz) / wellbore_radius_));
            return well_index_i;
        }

        double WellIndexCalculator::dir_wellblock_radius(double dx, double dy, double kx, double ky) {
            double r = 0.28 * sqrt((dx * dx) * sqrt(ky / kx) + (dy * dy) * sqrt(kx / ky)) /
                       (sqrt(sqrt(kx / ky)) + sqrt(sqrt(ky / kx)));
            return r;
        }

        QList<IntersectedCell> WellIndexCalculator::compute_well_indices() {
            assert(spline_points().length() == 2);
            // Find intersected blocks and the points of intersection
            // \todo Call this in a loop. When there is more than two ponts defining the spline, ensure that the blocks are not duplicated when going from one segment to the next.
            QList<IntersectedCell> intersected_cells = cells_intersected();

            for (auto cell : intersected_cells) {
                cell.set_well_index(well_index_cell(cell));
            }
            return intersected_cells;
        }
    }
}
