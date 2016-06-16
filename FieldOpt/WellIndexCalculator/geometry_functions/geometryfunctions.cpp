#include "geometryfunctions.h"

namespace WellIndexCalculator {
    namespace GeometryFunctions {
        using namespace Eigen;

        Vector3d line_plane_intersection(Vector3d p0, Vector3d p1, Vector3d normal_vector, Vector3d point_in_plane) {

            Vector3d line_vector = p1 - p0;
            line_vector.normalize();
            normal_vector.normalize();
            auto w = p0 - point_in_plane;
            double s = normal_vector.dot(-w) / normal_vector.dot(line_vector);
            return p0 + s*line_vector;
        }

        bool point_on_same_side(Vector3d point, Vector3d plane_point, Vector3d normal_vector, double slack) {
            double dot_product = (point - plane_point).dot(normal_vector);
            return dot_product >= 0.0 - slack;
        }

        QPair<QList<int>, QList<Vector3d>> cells_intersected(Vector3d start_point, Vector3d end_point,
                                                             Reservoir::Grid::Grid *grid) {
            QList<int> global_cell_indeces;
            QList<Vector3d> entry_points;

            // Add first and last cell blocks to the lists
            Reservoir::Grid::Cell last_cell = grid->GetCellEnvelopingPoint(end_point);
            Reservoir::Grid::Cell first_cell = grid->GetCellEnvelopingPoint(start_point);
            int last_cell_index = last_cell.global_index();
            int first_cell_index = first_cell.global_index();
            global_cell_indeces.append(first_cell_index);
            entry_points.append(start_point);

            // If the first and last blocks are the same, return the block and start+end points
            if (last_cell_index == first_cell_index) {
                entry_points.append(end_point);
                return QPair<QList<int>, QList<Vector3d>>(global_cell_indeces, entry_points);
            }


            Vector3d exit_point = find_exit_point(first_cell, start_point, end_point, start_point);
            // Make sure we follow line in the correct direction. (i.e. dot product positive)
            if ((end_point - start_point).dot(exit_point - start_point) <= 0.0) {
                exit_point = find_exit_point(first_cell, start_point, end_point, exit_point);
            }

            double epsilon = 0.01 / (end_point - exit_point).norm();
            Vector3d move_exit_epsilon = exit_point * (1 - epsilon) + end_point * epsilon;
            Reservoir::Grid::Cell current_cell = grid->GetCellEnvelopingPoint(move_exit_epsilon);
            double epsilon_temp = epsilon;

            // Move untill we're out of the first cell
            while (current_cell.global_index() == first_cell_index) {
                epsilon_temp = 10 * epsilon_temp;
                move_exit_epsilon = exit_point * (1 - epsilon_temp) + end_point * epsilon_temp;
                current_cell = grid->GetCellEnvelopingPoint(move_exit_epsilon);
            }

            // Add previous exit point to list, find next exit point and all other up to the end_point
            while (current_cell.global_index() != last_cell_index) {
                global_cell_indeces.append(current_cell.global_index());
                entry_points.append(exit_point);
                exit_point = find_exit_point(current_cell, exit_point, end_point, exit_point);

                if (exit_point == end_point) { // Terminate loop if the exit point is the last point
                    current_cell = last_cell;
                }
                else { // Move slightly along line to enter the new cell and get it
                    move_exit_epsilon = exit_point * (1 - epsilon) + end_point * epsilon;
                    current_cell = grid->GetCellEnvelopingPoint(move_exit_epsilon);
                }
            }
            global_cell_indeces.append(last_cell_index);
            entry_points.append(exit_point);
            entry_points.append(end_point);
            return QPair<QList<int>, QList<Vector3d>>(global_cell_indeces, entry_points);
        }

        Vector3d find_exit_point(Reservoir::Grid::Cell cell,
                                 Vector3d entry_point, Vector3d end_point, Vector3d exception_point) {
            Vector3d line = end_point - entry_point;

            // Loop through the cell faces untill we find one that the line intersects
            for (Reservoir::Grid::Cell::Plane plane : cell.planes()) {
                if (plane.normal_vector.dot(line) != 0) { // Check that the line and face are not parallel.
                    auto intersect_point = line_plane_intersection(entry_point, end_point, plane.normal_vector, plane.corners[0]);

                    // Check that the intersect point is on the correct side of all faces (i.e. inside the cell)
                    bool feasible_point = true;
                    for (auto p : cell.planes()) {
                        if (!point_on_same_side(intersect_point, p.corners[0], p.normal_vector, 10e-6)) {
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

        double well_index_cell(Reservoir::Grid::Cell cell, QList<Vector3d> start_points,
                               QList<Vector3d> end_points, double wellbore_radius) {

            // Determine the 3 (orthogonal, or very close to orth.) vectors to project line onto.
            // Corners 4&5, 4&6 and 4&0 span the cell from the front bottom left corner.
            Vector3d xvec = cell.corners()[5] - cell.corners()[4];
            Vector3d yvec = cell.corners()[6] - cell.corners()[4];
            Vector3d zvec = cell.corners()[0] - cell.corners()[4];

            // Finds the dimensional sizes (i.e. length in each direction) of the cell block
            double dx = xvec.norm();
            double dy = yvec.norm();
            double dz = zvec.norm();

            // Get directional permeabilities
            double kx = cell.permx();
            double ky = cell.permy();
            double kz = cell.permz();

            double Lx = 0;
            double Ly = 0;
            double Lz = 0;

            for (int ii = 0; ii < start_points.length(); ++ii) { // Current segment ii
                // Compute vector from segment
                Vector3d current_vec = end_points.at(ii) - start_points.at(ii);

                /* Proejcts segment vector to directional spanning vectors and determines the length.
                 * of the projections. Note that we only only care about the length of the projection,
                 * not the spatial position. Also adds the lengths of previous segments in case there
                 * is more than one segment within the well.
                 */
                Lx = Lx + (xvec * xvec.dot(current_vec) / xvec.dot(xvec)).norm();
                Ly = Ly + (yvec * yvec.dot(current_vec) / yvec.dot(yvec)).norm();
                Lz = Lz + (zvec * zvec.dot(current_vec) / zvec.dot(zvec)).norm();
            }

            // Compute Well Index from formula provided by Shu
            double well_index_x = (dir_well_index(Lx, dy, dz, ky, kz, wellbore_radius));
            double well_index_y = (dir_well_index(Ly, dx, dz, kx, kz, wellbore_radius));
            double well_index_z = (dir_well_index(Lz, dx, dy, kx, ky, wellbore_radius));
            return sqrt(well_index_x * well_index_x + well_index_y * well_index_y + well_index_z * well_index_z);
        }

        double dir_well_index(double Lx, double dy, double dz, double ky, double kz, double wellbore_radius) {
            double silly_eclipse_factor = 0.008527;
            double well_index_i = silly_eclipse_factor * (2 * M_PI * sqrt(ky * kz) * Lx) /
                                  (log(dir_wellblock_radius(dy, dz, ky, kz) / wellbore_radius));
            return well_index_i;
        }

        double dir_wellblock_radius(double dx, double dy, double kx, double ky) {
            double r = 0.28 * sqrt((dx * dx) * sqrt(ky / kx) + (dy * dy) * sqrt(kx / ky)) /
                       (sqrt(sqrt(kx / ky)) + sqrt(sqrt(ky / kx)));
            return r;
        }

        QPair<QList<int>, QList<double> > well_index_of_grid(Reservoir::Grid::Grid *grid, QList<Vector3d> start_points,
                                                             QList<Vector3d> end_points, double wellbore_radius) {
            // Find intersected blocks and the points of intersection
            QPair<QList<int>, QList<Vector3d>> temp_pair = cells_intersected(start_points[0], end_points[0], grid);

            QList<double> well_indeces;
            for (int ii = 0; ii < temp_pair.first.length(); ii++) {
                QList<Vector3d> entry_points = {temp_pair.second[ii]};
                QList<Vector3d> exit_points = {temp_pair.second[ii + 1]};
                well_indeces.append(
                        well_index_cell(grid->GetCell(temp_pair.first[ii]),
                                        entry_points, exit_points, wellbore_radius));
            }
            return QPair<QList<int>, QList<double>>(temp_pair.first, well_indeces);
        }

    }
}
