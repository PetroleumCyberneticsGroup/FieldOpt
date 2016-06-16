#include "geometryfunctions.h"
#include "well_constraint_projections.h"

namespace WellIndexCalculator {
    namespace WellConstraintProjections {
        using namespace Eigen;

        Vector3d point_to_cell_shortest(Reservoir::Grid::Cell cell, Vector3d point) {
            if (cell.EnvelopsPoint(point)) {
                return point;
            }

            // Shortest distance so far
            double minimum = INFINITY;
            Vector3d closest_point = point;

            for (auto face : cell.faces()) {
                Vector3d temp_point = point_to_face_shortest(face, point, cell);
                Vector3d projected_length = point - temp_point;
                if (projected_length.norm() < minimum) {
                    closest_point = temp_point;
                    minimum = projected_length.norm();
                }
            }
            return closest_point;
        }

        Vector3d point_to_face_shortest(Reservoir::Grid::Cell::Face face, Vector3d point, Reservoir::Grid::Cell cell) {
            // Calculate normal vector and normalize
            auto n_vec = face.normal_vector.normalized();

            // Project point onto plane spanned by face
            Vector3d proj_point = point - n_vec.dot(point - face.corners[0]) * n_vec;

            // Check if the point is inside the face (checking if it is inside the cell is equivalent here)
            if (cell.EnvelopsPoint(proj_point)) {
                return proj_point;
            }

            // If the above is false, projected point is outside face, The closest point lies on one of the four lines.
            // We create an array containing the index for line segments in the face, and loop through the lines to
            // find best point.
            int line_indices[4][2] = {{0, 1},
                                      {1, 3},
                                      {3, 2},
                                      {2, 0}};
            double minimum = INFINITY;
            Vector3d closest_point;
            for (int ii = 0; ii < 4; ii++) {
                QList<Vector3d> temp_line({face.corners[line_indices[ii][0]], face.corners[line_indices[ii][1]]});
                Vector3d temp_point = point_to_line_shortest(temp_line, point);
                Vector3d projected_length = point - temp_point;

                if (projected_length.norm() < minimum) {
                    closest_point = temp_point;
                    minimum = projected_length.norm();
                }
            }
            return closest_point;
        }

        Vector3d point_to_line_shortest(QList<Vector3d> line_segment, Vector3d P0) {
            auto closest_p_q = closest_points_on_lines(P0, P0, line_segment[0], line_segment[1]);
            return closest_p_q.second;
        }

        Vector3d non_inv_quad_coeffs(Vector3d x, Vector3d n) {
            Vector3d coeffs;
            coeffs(0) = n(0) * n(0) + n(1) * n(1) + n(2) * n(2);
            coeffs(1) = 2 * (x(0) * n(0) + x(1) * n(1) + x(2) * n(2));
            coeffs(2) = x(0) * x(0) + x(1) * x(1) + x(2) * x(2) - 1;
            return coeffs;
        }

        Vector3d rm_entries_eps(Vector3d m, double eps) {
            for (int ii = 0; ii < 3; ii++) {
                if (fabs(m[ii]) < eps) {
                    m(ii) = 0;
                }
            }
            return m;
        }

        Matrix3d rm_entries_eps_matrix(Matrix3d m, double eps) {
            for (int ii = 0; ii < 3; ii++) {
                for (int jj = 0; jj < 3; jj++) {
                    if (fabs(m(ii, jj)) < eps) {
                        m(ii, jj) = 0;
                    }
                }
            }
            return m;
        }

        VectorXd rm_entries_eps_coeffs(VectorXd m, double eps) {
            for (int ii = 0; ii < 7; ii++) {
                if (fabs(m[ii]) < eps) {
                    m(ii) = 0;
                }
            }
            return m;
        }


        QList<Vector3d> interwell_constraint_projection(QList<Vector3d> coords, double d) {
            // If the two line segments already satisfy the interwell distance constraint, return the coordinates.
            if (shortest_distance(coords) >= d) {
                return coords;
            }
            QList<Vector3d> solution_coords, moved_coords, temp_coords;

            /* Iterate through moving points. First try moving 2 points, then 3 points
             * then 4 points. If problem can be solved moving k points, moving k+1 points
             * will be a worse solution. Return the best k point solution.
             */
            double cost = INFINITY;

            // ################## 2 POINT PART ############################
            // ################ END 2 POINT PART ##########################
            int two_point_index[4][2] = {{0, 2},
                                         {0, 3},
                                         {1, 2},
                                         {1, 3}};

            for (int ii = 0; ii < 4; ii++) {
                moved_coords = coords;
                temp_coords = well_length_projection(coords[two_point_index[ii][0]],
                                                     coords[two_point_index[ii][1]],
                                                     INFINITY, d, 10e-5);
                moved_coords.replace(two_point_index[ii][0], temp_coords[0]);
                moved_coords.replace(two_point_index[ii][1], temp_coords[1]);
                if (shortest_distance(moved_coords) >= d &&
                    movement_cost(coords, moved_coords) < cost) {
                    // If several moves of two points work, save the one with lowest movement cost
                    cost = movement_cost(coords, moved_coords);
                    solution_coords = moved_coords;
                }
            }
            // If there were any succesful configurations, return the best one.
            if (cost < INFINITY) {
                return solution_coords;
            }
            // ################ END 2 POINT PART ##########################

            // ################## 3 POINT PART ############################
            // If no 2 point movements were succesful, try moving 3 points.
            int three_point_index[4][3] = {{2, 0, 1},
                                           {3, 0, 1},
                                           {0, 2, 3},
                                           {1, 2, 3}};
            for (int ii = 0; ii < 4; ii++) {
                // Reset moved coords to initial state
                moved_coords = coords;

                // Choose which 3 points to move. (order is important, second and third entry should belong to same line segment)
                QList<Vector3d> input_cords_3p;
                for (int jj = 0; jj < 3; jj++) {
                    input_cords_3p.append(coords.at(three_point_index[ii][jj]));
                }
                Matrix3d temp_A = build_A_3p(input_cords_3p);
                Vector3d temp_b = build_b_3p(input_cords_3p, d);

                /* The kkt_eg_solutions solver handles some numerical issues
                 * like A having some values close to machine epsilon and
                 * eigenvalues being close to 0. Just assume that any solution
                 * must be among the ones given in solution candidates. we check
                 * all of them.
                 */
                QList<Vector3d> solution_candidates = kkt_eq_solutions(temp_A, temp_b);

                for (int sol_num = 0; sol_num < solution_candidates.length(); sol_num++) {
                    // Solution of three point problem
                    temp_coords = move_points_3p(input_cords_3p, d,
                                                 solution_candidates.at(sol_num));
                    if (temp_coords.length() < 1) { temp_coords = input_cords_3p; }

                    for (int jj = 0; jj < 3; jj++) {
                        moved_coords.replace(three_point_index[ii][jj], temp_coords.at(jj));
                    }

                    if (shortest_distance(moved_coords) >= d - 0.001 &&
                        movement_cost(coords, moved_coords) < cost) {
                        // If several moves of three points work, save the one with lovest movement cost
                        cost = movement_cost(coords, moved_coords);
                        solution_coords = moved_coords;
                    }
                }

            }
            // If there were any succesful configurations, return the best one.
            if (cost < INFINITY) {
                std::cout << "Found 3-point solution" << std::endl;
                return solution_coords;
            }
            // ################## END 3 POINT PART ########################



            // ################## 4 POINT PART ############################
            std::cout << "Found no 3-point solution. Try 4 points" << std::endl;

            // Get all candidates for vector s
            /* The kkt_eg_solutions solver handles some numerical issues
             * like A having some values close to machine epsilon and
             * eigenvalues being close to 0. Just assume that any solution
             * must be among the ones given in solution candidates. we check
             * all of them.
             */
            Matrix3d temp_A = build_A_4p(coords);
            Vector3d temp_b = build_b_4p(coords, d);
            QList<Vector3d> solution_candidates = kkt_eq_solutions(temp_A, temp_b);

            // Go through candidates s and pick the best one
            for (int sol_num = 0; sol_num < solution_candidates.length(); sol_num++) {

                moved_coords = move_points_4p(coords, d, solution_candidates.at(sol_num));
                if (shortest_distance(moved_coords) >= d - 0.001 &&
                    movement_cost(coords, moved_coords) < cost) {
                    // If several candidates for s work, save the one with lovest movement cost
                    cost = movement_cost(coords, moved_coords);
                    solution_coords = moved_coords;
                }
            }

            if (solution_coords.length() == 0)
                std::cout << "Found no solution to interwell projection problem" << std::endl;
            return solution_coords;
        }

        double shortest_distance_n_wells(QList<QList<Vector3d>> wells, int n) {
            double distance = INFINITY;

            // for all pairs of wells (i,j) i != j
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    // Create QList with current pair of wells
                    QList<Vector3d> current_pair;
                    current_pair.append(wells[i][0]);
                    current_pair.append(wells[i][1]);
                    current_pair.append(wells[j][0]);
                    current_pair.append(wells[j][1]);

                    if (shortest_distance(current_pair) < distance) {
                        distance = shortest_distance(current_pair);
                    }
                }
            }
            return distance;
        }

        QList<QList<Vector3d>> interwell_constraint_multiple_wells(QList<QList<Vector3d>> wells, double d, double tol) {
            double shortest_distance = 0;

            // Loop through all wells as long as some pair of wells violate inter-well distance
            // constraint. Tolerance tol added for quicker convergence.
            int max_iter = 10000;
            int iter = 0;
            while (shortest_distance < d - tol && iter < max_iter) {
                // for all pairs of wells (i,j) i != j
                for (int i = 0; i < wells.length(); i++) {
                    for (int j = i + 1; j < wells.length(); j++) {
                        // Create QList with current pair of wells
                        QList<Vector3d> current_pair;
                        current_pair.append(wells[i][0]);
                        current_pair.append(wells[i][1]);
                        current_pair.append(wells[j][0]);
                        current_pair.append(wells[j][1]);

                        // Project pair of wells
                        current_pair = interwell_constraint_projection(current_pair, d);

                        // Replace initial well pair with projected pair.
                        wells[i].replace(0, current_pair[0]);
                        wells[i].replace(1, current_pair[1]);
                        wells[j].replace(0, current_pair[2]);
                        wells[j].replace(1, current_pair[3]);
                    }
                }
                shortest_distance = shortest_distance_n_wells(wells, wells.length());
                iter += 1;
            }
            if (iter == max_iter)
                std::cout << "No convergence in interwell distance constraints after max iterations "
                << iter << " reached" << std::endl;

            return wells;
        }

        QList<QList<Vector3d> > well_length_constraint_multiple_wells(QList<QList<Vector3d> > wells,
                                                                      double max, double min, double epsilon) {
            QList<QList<Vector3d>> projected_wells;
            for (int i = 0; i < wells.length(); i++) {
                Vector3d current_heel = wells[i][0];
                Vector3d current_toe = wells[i][1];
                projected_wells.append(well_length_projection(current_heel, current_toe, max, min, epsilon));
            }
            return projected_wells;
        }

        bool feasible_well_length(QList<QList<Vector3d>> wells, double max, double min, double tol) {
            bool is_feasible = true;
            for (int i = 0; i < wells.length(); i++) {
                double current_well_length;
                current_well_length = (wells[i][0] - wells[i][1]).norm();

                // If smaller than min or larger than max (with tolerance tol). not feasible
                if (current_well_length < min - tol || current_well_length > max + tol) {
                    is_feasible = false;
                }
            }
            return is_feasible;
        }

        bool feasible_interwell_distance(QList<QList<Vector3d>> wells, double d, double tol) {
            // Number of wells
            bool is_feasible = true;
            if (shortest_distance_n_wells(wells, wells.length()) < d - tol) {
                is_feasible = false;
            }
            return is_feasible;
        }

        QList<QList<Vector3d> > both_constraints_multiple_wells(QList<QList<Vector3d> > wells, double d,
                                                                double tol, double max, double min, double epsilon) {
            int iter = 0;

            // While at least one of the constraints is violated, continue projecting
            while (!feasible_interwell_distance(wells, d, 3 * tol) ||
                   !feasible_well_length(wells, max, min, tol)) {
                wells = well_length_constraint_multiple_wells(wells, max, min, epsilon);
                wells = interwell_constraint_multiple_wells(wells, d, tol);

                iter += 1;
                if (iter > 100) {
                    std::cout << "In both_both_constraints_multiple_wells: above max number of iterations" << std::endl;
                    return wells;
                }
            }
            return wells;
        }

        Vector3d well_domain_constraint(Vector3d point, QList<Reservoir::Grid::Cell> cells) {
            double minimum = INFINITY;
            Vector3d best_point;

            for (auto cell : cells) {
                Vector3d temp_point = point_to_cell_shortest(cell, point);
                Vector3d projected_length = point - temp_point;

                if (projected_length.norm() < minimum) {
                    best_point = temp_point;
                    minimum = projected_length.norm();
                }
            }
            return best_point;
        }

        Vector3d well_domain_constraint_indices(Vector3d point, Reservoir::Grid::Grid *grid, QList<int> index_list) {
            QList<Reservoir::Grid::Cell> cells;
            for (int index : index_list) {
                cells.append(grid->GetCell(index));
            }
            return well_domain_constraint(point, cells);
        }

        double shortest_distance(QList<Vector3d> coords) {
            auto closest_p_q = closest_points_on_lines(coords[0], coords[1], coords[2], coords[3]);
            Vector3d closest_distance_vec = closest_p_q.second - closest_p_q.first;
            double distance = sqrt(closest_distance_vec.transpose() * closest_distance_vec);
            return distance;
        }

        QList<Vector3d> well_length_projection(Vector3d heel, Vector3d toe, double max, double min, double epsilon) {
            QList<Vector3d> projected_points;
            Vector3d moved_heel;
            Vector3d moved_toe;

            // Need the vector going from heel to toe to project points
            Vector3d heel_to_toe_vec = toe - heel;
            // Distance between heel and toe.
            double d = heel_to_toe_vec.norm();

            // heel and toe same point.
            // All directions equally good.
            if (d == 0) {
                Vector3d unit_vector;
                unit_vector << 1, 0, 0;
                moved_heel = heel + (min / 2) * unit_vector;
                moved_toe = heel - (min / 2) * unit_vector;
                projected_points.append(moved_heel);
                projected_points.append(moved_toe);
                return projected_points;
            }

            // Normalize vector to get correct distance
            heel_to_toe_vec.normalize();

            // Trivial case
            if (d <= max && d >= min) {
                projected_points.append(heel);
                projected_points.append(toe);
            }

                // Distance too long
            else if (d > max) {
                double move_distance = 0.5 * (d - max + (epsilon / 2));
                moved_heel = heel + move_distance * heel_to_toe_vec;
                moved_toe = toe - move_distance * heel_to_toe_vec;
                projected_points.append(moved_heel);
                projected_points.append(moved_toe);
            }

                // Distance too short
            else {
                double move_distance = 0.5 * (d - min - (epsilon / 2));
                moved_heel = heel + move_distance * heel_to_toe_vec;
                moved_toe = toe - move_distance * heel_to_toe_vec;
                projected_points.append(moved_heel);
                projected_points.append(moved_toe);
            }

            return projected_points;
        }

        QList<Vector3d> non_inv_solution(Matrix3d A, Vector3d b) {
            QList<Vector3d> solution_vectors;
            // Compute full pivotal LU decomposition of A
            FullPivLU<Matrix3d> lu(A);
            Vector3d x = A.fullPivLu().solve(b);

            MatrixXd nu = lu.kernel();
            Vector3d null_space;
            if (nu.cols() > 1) {
                null_space << nu(0, 0), nu(1, 0), nu(2, 0);
                std::cout << "Null space/kernel of (A-mu I) has more than 1 vector. " << std::endl;
            }
            else null_space = nu;

            VectorXd real_roots;
            VectorXd complex_roots;
            rpoly_plus_plus::FindPolynomialRootsJenkinsTraub(
                    non_inv_quad_coeffs(x, null_space), &real_roots, &complex_roots);


            if (complex_roots(0) == 0) {
                Vector3d s0 = x + real_roots(0) * null_space;
                solution_vectors.append(s0);
            }
            if (complex_roots(1) == 0) {
                Vector3d s1 = x + real_roots(1) * null_space;
                solution_vectors.append(s1);
            }

            return solution_vectors;
        }

        bool solution_existence(Matrix3d A, Vector3d b) {
            Vector3d x = A.fullPivLu().solve(b);
            return ((A * x).isApprox(b));

        }


        /* PUTTING ALL REWORKED EIGEN FUNCTIONS BELOW HERE*/

        Matrix3d build_A_4p(QList<Vector3d> coords) {
            Vector3d vec1 = coords.at(0);
            Vector3d vec2 = coords.at(1);
            Vector3d vec3 = coords.at(2);
            Vector3d vec4 = coords.at(3);
            Vector3d avg_vec = 0.25 * (vec1 + vec2 + vec3 + vec4);
            vec1 = vec1 - avg_vec;
            vec2 = vec2 - avg_vec;
            vec3 = vec3 - avg_vec;
            vec4 = vec4 - avg_vec;

            return vec1 * vec1.transpose() + vec2 * vec2.transpose() + vec3 * vec3.transpose() +
                   vec4 * vec4.transpose();
        }

        Vector3d build_b_4p(QList<Vector3d> coords, double d) {
            Vector3d vec1 = coords.at(0);
            Vector3d vec2 = coords.at(1);
            Vector3d vec3 = coords.at(2);
            Vector3d vec4 = coords.at(3);
            Vector3d avg_vec = 0.25 * (vec1 + vec2 + vec3 + vec4);
            vec1 = vec1 - avg_vec;
            vec2 = vec2 - avg_vec;
            vec3 = vec3 - avg_vec;
            vec4 = vec4 - avg_vec;

            return 0.5 * d * (vec1 + vec2 - vec3 - vec4);
        }

        Matrix3d build_A_3p(QList<Vector3d> coords) {
            Vector3d vec1 = coords.at(0);
            Vector3d vec2 = coords.at(1);
            Vector3d vec3 = coords.at(2);
            Vector3d avg_vec = (1.0 / 3) * (vec1 + vec2 + vec3);

            vec1 = vec1 - avg_vec;
            vec2 = vec2 - avg_vec;
            vec3 = vec3 - avg_vec;
            return vec1 * vec1.transpose() + vec2 * vec2.transpose() + vec3 * vec3.transpose();
        }

        Vector3d build_b_3p(QList<Vector3d> coords, double d) {
            Vector3d vec1 = coords.at(0);
            Vector3d vec2 = coords.at(1);
            Vector3d vec3 = coords.at(2);
            Vector3d avg_vec = (1.0 / 3) * (vec1 + vec2 + vec3);
            vec1 = vec1 - avg_vec;
            vec2 = vec2 - avg_vec;
            vec3 = vec3 - avg_vec;

            return (2.0 / 3) * d * (vec1) - (1.0 / 3) * d * (vec2 + vec3);
        }

        VectorXd coeff_vector(Vector3d D, Matrix3d Qinv, Vector3d b) {
            double D1 = D(0);
            double D2 = D(1);
            double D3 = D(2);
            double sum_i = D1 + D2 + D3;
            double sum_ij = D1*D2 + D2*D3 + D3*D1;
            double prod_i = D1*D2*D3;
            double Qtb_1 = Qinv.row(0) * b * (Qinv.row(0) * b);
            double Qtb_2 = Qinv.row(1) * b * (Qinv.row(1) * b);
            double Qtb_3 = Qinv.row(2) * b * (Qinv.row(2) * b);

            VectorXd lambda(7);
            lambda(0) = 1;
            lambda(1) = -2 * sum_i;
            lambda(2) = 2 * sum_ij + sum_i * sum_i - (Qtb_1 + Qtb_2 + Qtb_3);
            lambda(3) = -2 * prod_i - 2 * sum_i * sum_ij - Qtb_1 * (-2 * D2 - 2 * D3)
                        - Qtb_2 * (-2 * D3 - 2 * D1)
                        - Qtb_3 * (-2 * D1 - 2 * D2);
            lambda(4) = 2 * sum_i * prod_i + sum_ij * sum_ij - Qtb_1 * (D2 * D2 + D3 * D3 + 4 * D2 * D3)
                        - Qtb_2 * (D3 * D3 + D1 * D1 + 4 * D3 * D1)
                        - Qtb_3 * (D1 * D1 + D2 * D2 + 4 * D1 * D2);
            lambda(5) = -2 * sum_ij * prod_i - Qtb_1 * (-2 * D2 * D3 * D3 - 2 * D3 * D2 * D2)
                        - Qtb_2 * (-2 * D3 * D1 * D1 - 2 * D1 * D3 * D3)
                        - Qtb_3 * (-2 * D1 * D2 * D2 - 2 * D2 * D1 * D1);
            lambda(6) = prod_i * prod_i - Qtb_1 * (D2 * D2 * D3 * D3)
                        - Qtb_2 * (D3 * D3 * D1 * D1)
                        - Qtb_3 * (D1 * D1 * D2 * D2);

            return lambda;
        }

        double movement_cost(QList<Vector3d> old_coords, QList<Vector3d> new_coords) {
            double n_of_points = old_coords.length();
            if (new_coords.length() != n_of_points) {
                throw std::runtime_error("Error in movement_cost: Lists of points are not the same length");
            }
            double cost_squares;
            for (int ii = 0; ii < n_of_points; ii++) {
                cost_squares += (old_coords.at(ii) - new_coords.at(ii)).squaredNorm();
            }

            return cost_squares;
        }

        QList<Vector3d> move_points_4p(QList<Vector3d> coords, double d, Vector3d s) {
            // Normalize s in case it is not of unit length
            s.normalize();
            Vector3d avg_point = 0.25 * (coords.at(0) + coords.at(1) + coords.at(2) + coords.at(3));
            Vector3d top_plane_point = avg_point + (d / 2) * (s);
            Vector3d bot_plane_point = avg_point - (d / 2) * (s);

            Vector3d x0moved = project_point_to_plane(coords.at(0), s,
                                                      top_plane_point);
            Vector3d x1moved = project_point_to_plane(coords.at(1), s,
                                                      top_plane_point);
            Vector3d x2moved = project_point_to_plane(coords.at(2), s,
                                                      bot_plane_point);
            Vector3d x3moved = project_point_to_plane(coords.at(3), s,
                                                      bot_plane_point);

            QList<Vector3d> moved_coords;
            moved_coords.append(x0moved);
            moved_coords.append(x1moved);
            moved_coords.append(x2moved);
            moved_coords.append(x3moved);
            return moved_coords;
        }

        QList<Vector3d> move_points_3p(QList<Vector3d> coords, double d, Vector3d s) {
            // Normalize s in case it is not of unit length
            s.normalize();
            Vector3d avg_point = (1.0 / 3) * (coords.at(0) + coords.at(1) + coords.at(2));
            Vector3d top_plane_point = avg_point + (2.0 * d / 3) * (s);
            Vector3d bot_plane_point = avg_point - (1.0 * d / 3) * (s);

            Vector3d x0moved = project_point_to_plane(coords.at(0), s,
                                                      top_plane_point);
            Vector3d x1moved = project_point_to_plane(coords.at(1), s,
                                                      bot_plane_point);
            Vector3d x2moved = project_point_to_plane(coords.at(2), s,
                                                      bot_plane_point);


            QList<Vector3d> moved_coords;
            moved_coords.append(x0moved);
            moved_coords.append(x1moved);
            moved_coords.append(x2moved);
            return moved_coords;
        }

        Vector3d project_point_to_plane(Vector3d point, Vector3d normal_vector, Vector3d plane_point) {
            Vector3d proj_point = point - normal_vector * ((point - plane_point).transpose() * normal_vector);
            return proj_point;

        }

        QList<Vector3d> kkt_eq_solutions(Matrix3d A, Vector3d b) {

            QList<Vector3d> candidate_solutions;

            /* First assume that A-\mu I has an inverse.
             * We can find the inverse of it and solve
             * a sixth degree equation for \mu.
             */
            A = rm_entries_eps_matrix(A, 10e-12);
            SelfAdjointEigenSolver<Matrix3d> A_es(A);

            // Need to remove eigenvalues which are approx 0
            Vector3d eigenvalues = rm_entries_eps(A_es.eigenvalues(), 10e-12);
            /*std::cout << "eigvalues" << std::endl << A_es.eigenvalues() << std::endl;
            std::cout << "eigvalues small removed" << std::endl << eigenvalues << std::endl;
            std::cout << "qinvb" << std::endl << A_es.eigenvectors().inverse()*b << std::endl;*/
            // Compute coefficients of 6th degree polynomial
            VectorXd coeffs = coeff_vector(eigenvalues, A_es.eigenvectors().inverse(),
                                           b);

            /* There is an issue where coefficients should be zero but are not
             * but because of numerical issues these need to be handled manually.
             * Simply set all whose fabs(x)<10-e12 to zero.
             */
            coeffs = rm_entries_eps_coeffs(coeffs, 10e-12);


            // Compute roots of polynomial
            VectorXd realroots(6);
            VectorXd comproots(6);
            rpoly_plus_plus::FindPolynomialRootsJenkinsTraub(coeffs, &realroots, &comproots);

            //std::cout << "polynomial coeffs = " << std::endl << coeffs << std::endl;

            //Matrix3d invmatr = Matrix3d::Zero();

            for (int ii = 0; ii < 6; ii++) {

                // Root may not be complex or an eigenvalue of A
                if (comproots[ii] == 0 && eigenvalues[0] != realroots[ii] &&
                    eigenvalues[1] != realroots[ii] && eigenvalues[2] != realroots[ii]) {

                    // We have found a valid root. Get vector s.
                    double cur_root = realroots[ii];
                    Vector3d cur_root_vec;
                    cur_root_vec << cur_root, cur_root, cur_root;
                    Matrix3d invmatr = (eigenvalues - cur_root_vec).asDiagonal();

                    //invmatr(0,0) = 1.00/(A_es.eigenvalues()[0]-cur_root);
                    //invmatr(1,1) = 1.00/(A_es.eigenvalues()[1]-cur_root);
                    //invmatr(2,2) = 1.00/(A_es.eigenvalues()[2]-cur_root);
                    Vector3d s = A_es.eigenvectors() * invmatr.inverse() * A_es.eigenvectors().inverse() * b;

                    candidate_solutions.append(s);
                }
            }

            /* Now for the second part assume that A-\mu I is not
             * invertible, i.e. \mu is an eigenvalue of A. Then
             * we either have an infinite amount of solutions of
             * (A-\mu I)s = b. Require s have length 1 to find
             * at most two solutions as long as all points are
             * not on the same line.
             */

            // Loop through all 3 eigenvalues of A
            for (int i = 0; i < 3; i++) {

                QList<Vector3d> eigenvalue_solutions;

                // Create linear system (A-\my I)s = b
                Matrix3d A_eig = A - eigenvalues[i] * Matrix3d::Identity();

                /*
                A_eig << A(0,0)-eigval(i), A(0,1)          , A(0,2),
                         A(1,0)          , A(1,1)-eigval(i), A(1,2),
                         A(2,0)          , A(2,1)           ,A(2,2)-eigval(i);
                */
                Vector3d b_eig = b;
                // Check for existence of solution
                //std::cout << "eigenvalue number "<< i << " = " << eigenvalues[i] << std::endl;
                if (solution_existence(A_eig, b_eig)) {
                    //std::cout << "Indeed solvable" << std::endl;
                    eigenvalue_solutions = non_inv_solution(A_eig, b_eig);
                }

                // If any solutions, add them to solution_vectors
                for (int jj = 0; jj < eigenvalue_solutions.length(); jj++) {
                    candidate_solutions.append(eigenvalue_solutions.at(jj));
                    //std::cout << candidate_solutions.length() <<" solution found. mu eigenvalue of A. solution is" << std::endl << eigenvalue_solutions.at(jj) << std::endl;
                }
            }

            return candidate_solutions;
        }

        QPair<Vector3d, Vector3d> closest_points_on_lines(Vector3d P0, Vector3d P1, Vector3d Q0, Vector3d Q1) {
            /* Function runs through all possible combinations of where the two closest points could be located.
             * This function is a slightly edited version of the one from:
             * http://www.geometrictools.com/GTEngine/Include/Mathematics/GteDistSegmentSegmentExact.h
             *
             * David Eberly, Geometric Tools, Redmond WA 98052
             * Copyright (c) 1998-2016
             * Distributed under the Boost Software License, Version 1.0.
             * http://www.boost.org/LICENSE_1_0.txt
             * http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
             * File Version: 2.1.0 (2016/01/25)d
             */
            Vector3d P1mP0 = P1 - P0;
            Vector3d Q1mQ0 = Q1 - Q0;
            Vector3d P0mQ0 = P0 - Q0;

            double a = P1mP0.transpose() * P1mP0;
            double b = P1mP0.transpose() * Q1mQ0;
            double c = Q1mQ0.transpose() * Q1mQ0;
            double d = P1mP0.transpose() * P0mQ0;
            double e = Q1mQ0.transpose() * P0mQ0;
            double const zero = 0;
            double const one = 1;
            double det = a * c - b * b;
            double s, t, nd, bmd, bte, ctd, bpe, ate, btd;

            if (det > zero) {
                bte = b * e;
                ctd = c * d;
                if (bte <= ctd) {  // s <= 0
                    s = zero;
                    if (e <= zero) { // t <= 0, region 6
                        t = zero;
                        nd = -d;
                        if (nd >= a)
                            s = one;
                        else if (nd > zero)
                            s = nd / a;
                        // else: s is already zero
                    }
                    else if (e < c) { // 0 < t < 1, region 5
                        t = e / c;
                    }
                    else { // t >= 1, region 4
                        t = one;
                        bmd = b - d;
                        if (bmd >= a)
                            s = one;
                        else if (bmd > zero)
                            s = bmd / a;
                        // else:  s is already zero
                    }
                }
                else { // s > 0
                    s = bte - ctd;
                    if (s >= det) { // s >= 1
                        s = one; // s = 1
                        bpe = b + e;
                        if (bpe <= zero) { // t <= 0, region 8
                            t = zero;
                            nd = -d;
                            if (nd <= zero)
                                s = zero;
                            else if (nd < a)
                                s = nd / a;
                            // else: s is already one
                        }
                        else if (bpe < c) { // 0 < t < 1, region 1
                            t = bpe / c;
                        }
                        else { // t >= 1, region 2
                            t = one;
                            bmd = b - d;
                            if (bmd <= zero)
                                s = zero;
                            else if (bmd < a)
                                s = bmd / a;
                            // else:  s is already one
                        }
                    }
                    else { // 0 < s < 1
                        ate = a * e;
                        btd = b * d;
                        if (ate <= btd) { // t <= 0, region 7
                            t = zero;
                            nd = -d;
                            if (nd <= zero)
                                s = zero;
                            else if (nd >= a)
                                s = one;
                            else
                                s = nd / a;
                        }
                        else { // t > 0
                            t = ate - btd;
                            if (t >= det) { // t >= 1, region 3
                                t = one;
                                bmd = b - d;
                                if (bmd <= zero)
                                    s = zero;
                                else if (bmd >= a)
                                    s = one;
                                else
                                    s = bmd / a;
                            }
                            else { // 0 < t < 1, region 0
                                s /= det;
                                t /= det;
                            }
                        }
                    }
                }
            }
            else {
                // The segments are parallel.  The quadratic factors to R(s,t) =
                // a*(s-(b/a)*t)^2 + 2*d*(s - (b/a)*t) + f, where a*c = b^2,
                // e = b*d/a, f = |P0-Q0|^2, and b is not zero.  R is constant along
                // lines of the form s-(b/a)*t = k, and the minimum of R occurs on the
                // line a*s - b*t + d = 0.  This line must intersect both the s-axis
                // and the t-axis because 'a' and 'b' are not zero.  Because of
                // parallelism, the line is also represented by -b*s + c*t - e = 0.
                //
                // The code determines an edge of the domain [0,1]^2 that intersects
                // the minimum line, or if none of the edges intersect, it determines
                // the closest corner to the minimum line.  The conditionals are
                // designed to test first for intersection with the t-axis (s = 0)
                // using -b*s + c*t - e = 0 and then with the s-axis (t = 0) using
                // a*s - b*t + d = 0.

                // When s = 0, solve c*t - e = 0 (t = e/c).
                if (e <= zero) { // t <= 0
                    // Now solve a*s - b*t + d = 0 for t = 0 (s = -d/a).
                    t = zero;
                    nd = -d;
                    if (nd <= zero) // s <= 0, region 6
                        s = zero;
                    else if (nd >= a) // s >= 1, region 8
                        s = one;
                    else // 0 < s < 1, region 7
                        s = nd / a;
                }
                else if (e >= c) { // t >= 1
                    // Now solve a*s - b*t + d = 0 for t = 1 (s = (b-d)/a).
                    t = one;
                    bmd = b - d;
                    if (bmd <= zero) // s <= 0, region 4
                        s = zero;
                    else if (bmd >= a) // s >= 1, region 2
                        s = one;
                    else // 0 < s < 1, region 3
                        s = bmd / a;
                }
                else { // 0 < t < 1
                    // The point (0,e/c) is on the line and domain, so we have one
                    // point at which R is a minimum.
                    s = zero;
                    t = e / c;
                }
            }

            Vector3d closest_P = P0 + s * P1mP0;
            Vector3d closest_Q = Q0 + t * Q1mQ0;
            return QPair<Eigen::Vector3d, Eigen::Vector3d>(closest_P, closest_Q);
        }


    }
}
