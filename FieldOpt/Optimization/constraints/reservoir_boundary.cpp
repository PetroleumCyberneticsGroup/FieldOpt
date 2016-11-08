#include "reservoir_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>

namespace Optimization {
    namespace Constraints {

        ReservoirBoundary::ReservoirBoundary(
                const Settings::Optimizer::Constraint &settings,
                Model::Properties::VariablePropertyContainer *variables,
                Reservoir::Grid::Grid *grid)
        {
            imin_ = settings.box_imin;
            imax_ = settings.box_imax;
            jmin_ = settings.box_jmin;
            jmax_ = settings.box_jmax;
            kmin_ = settings.box_kmin;
            kmax_ = settings.box_kmax;
            grid_ = grid;

            index_list_ = getListOfCellIndices();
            affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));

            int verbosity_level_ = 0;
            if (verbosity_level_>2){
                std::cout << "... ... initialized boundary constraint for well: "
                << settings.well.toStdString() << std::endl;
            }

            // QList with indices of box edge cells
            index_list_edge_ = getIndicesOfEdgeCells();

            // Debug: verbose level = 4
            if (verbosity_level_>3){
                std::cout << "... ... [debug] index_list_edge_ length (reservoir_boundary.cpp): "
                << index_list_edge_.length() << std::endl;
            }

            // Debug (move to tests):
            // for( int i=0; i < index_list_edge_.length(); ++i )
            // {
            //   std::cout << index_list_edge_[i] << std::endl;
            // }

            // TODO NOTE Figure out a more effective way to enforce the box
            // constraints (TASK A), then figure out way boundary constraints
            // for non-box (parallelogram) shapes (TASK B); finally, define this
            // constraint (out of ReservoirBoundary) as a standalone constraint
            // (call it Box) (TASK C)
            //
            // Steps for (A):
            // 1. find the edge cells of the box [x] + unit test [],
            //
            // 2. find the corner points of the entire box (assuming the box is a
            // parallelogram, which may not be true for any of the planes/faces
            // -> figure out how to deal with this later) [x] + unit test [],
            //
            // 3. print the box data to log for external visualization []
            //
            // 4. figure out if the current point is inside or outside
            // the box, e.g., create a BoxEnvelopsPoint function
            //
            // 5. if outside, project point onto nearest point on plane
            //
            // Steps for (B):
            // ...
        }

        void ReservoirBoundary::findCornerCells() {

            //GENERAL

            // box plane / cell face index ordering (viewed from above):

            //   2___3    6___7
            //   |   |    |   |
            //   |___|    |___|
            //   0   1    4   5

            // Comment: this appears to be the correct index ordering for
            // a grid cell. Check this ordering is consistent with the ordering
            // used in: 
            // /ConstraintMath/well_constraint_projections/well_constraint_projections.cpp

            // If now, how do the orderings correlate?

            // TODO: 
            // move the output messages from here to the unit test of this function.
            
            // ===============
            // UPPER BOX PLANE
            // ===============

            // UPPER BOX PLANE: LEFT EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        2___3
            //  -->   |   |
            //  -->   |___|
            //        0   1

            // Get corner cells of box
            QList<Eigen::Vector3d> upper_plane_left_bottom_cell_xyz = grid_->GetCell(imin_, jmin_, kmax_).corners();
            QList<Eigen::Vector3d> upper_plane_left_top_cell_xyz = grid_->GetCell(imin_, jmax_, kmax_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d upper_plane_left_bottom_corner_xyz = upper_plane_left_bottom_cell_xyz[0];
            Eigen::Vector3d upper_plane_left_top_corner_xyz = upper_plane_left_top_cell_xyz[2];
            // Print coordinates
            printCornerXYZ("upper plane, left edge, bottom corner: ", upper_plane_left_bottom_corner_xyz);
            printCornerXYZ("upper plane, left edge, top corner:    ", upper_plane_left_top_corner_xyz);

            // UPPER BOX PLANE: RIGHT EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        2___3
            //        |   |  <--
            //        |___|  <--
            //        0   1

            // Get corner cells of box
            QList<Eigen::Vector3d> upper_plane_right_bottom_cell_xyz = grid_->GetCell(imax_, jmin_, kmax_).corners();
            QList<Eigen::Vector3d> upper_plane_right_top_cell_xyz = grid_->GetCell(imax_, jmax_, kmax_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d upper_plane_right_bottom_corner_xyz = upper_plane_right_bottom_cell_xyz[1];
            Eigen::Vector3d upper_plane_right_top_corner_xyz = upper_plane_right_top_cell_xyz[3];
            // Print coordinates
            printCornerXYZ("upper plane, right edge, bottom corner:", upper_plane_right_bottom_corner_xyz);
            printCornerXYZ("upper plane, right edge, top corner:   ", upper_plane_right_top_corner_xyz);

            // UPPER BOX PLANE: BOTTOM EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        2___3
            //        |   |
            //        |___|
            //        0   1

            //         ^ ^
            //         | |

            // Get corner cells of box
            QList<Eigen::Vector3d> upper_plane_bottom_left_cell_xyz = grid_->GetCell(imin_, jmin_, kmax_).corners();
            QList<Eigen::Vector3d> upper_plane_bottom_right_cell_xyz = grid_->GetCell(imax_, jmin_, kmax_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d upper_plane_bottom_left_corner_xyz = upper_plane_bottom_left_cell_xyz[0];
            Eigen::Vector3d upper_plane_bottom_right_corner_xyz = upper_plane_bottom_right_cell_xyz[1];
            // Print coordinates
            printCornerXYZ("upper plane, bottom edge, left corner: ", upper_plane_bottom_left_corner_xyz);
            printCornerXYZ("upper plane, bottom edge, right corner:", upper_plane_bottom_right_corner_xyz);

            // UPPER BOX PLANE: TOP EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //         | |
            //         v v

            //        2___3
            //        |   |
            //        |___|
            //        0   1

            // Get corner cells of box
            QList<Eigen::Vector3d> upper_plane_top_left_cell_xyz = grid_->GetCell(imin_, jmax_, kmax_).corners();
            QList<Eigen::Vector3d> upper_plane_top_right_cell_xyz = grid_->GetCell(imax_, jmax_, kmax_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d upper_plane_top_left_corner_xyz = upper_plane_top_left_cell_xyz[2];
            Eigen::Vector3d upper_plane_top_right_corner_xyz = upper_plane_top_right_cell_xyz[3];
            // Print coordinates
            printCornerXYZ("upper plane, top edge, left corner:    ", upper_plane_top_left_corner_xyz);
            printCornerXYZ("upper plane, top edge, right corner:   ", upper_plane_top_right_corner_xyz);

            // ===============
            // LOWER BOX PLANE
            // ===============

            // LOWER BOX PLANE: LEFT EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        6___7
            //  -->   |   |
            //  -->   |___|
            //        4   5

            // Get corner cells of box
            QList<Eigen::Vector3d> lower_plane_left_bottom_cell_xyz = grid_->GetCell(imin_, jmin_, kmin_).corners();
            QList<Eigen::Vector3d> lower_plane_left_top_cell_xyz = grid_->GetCell(imin_, jmax_, kmin_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d lower_plane_left_bottom_corner_xyz = lower_plane_left_bottom_cell_xyz[4];
            Eigen::Vector3d lower_plane_left_top_corner_xyz = lower_plane_left_top_cell_xyz[6];
            // Print coordinates
            printCornerXYZ("lower plane, left edge, bottom corner: ", lower_plane_left_bottom_corner_xyz);
            printCornerXYZ("lower plane, left edge, top corner:    ", lower_plane_left_top_corner_xyz);

            // LOWER BOX PLANE: RIGHT EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        6___7
            //        |   |  <--
            //        |___|  <--
            //        4   5

            // Get corner cells of box
            QList<Eigen::Vector3d> lower_plane_right_bottom_cell_xyz = grid_->GetCell(imax_, jmin_, kmin_).corners();
            QList<Eigen::Vector3d> lower_plane_right_top_cell_xyz = grid_->GetCell(imax_, jmax_, kmin_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d lower_plane_right_bottom_corner_xyz = lower_plane_right_bottom_cell_xyz[5];
            Eigen::Vector3d lower_plane_right_top_corner_xyz = lower_plane_right_top_cell_xyz[7];
            // Print coordinates
            printCornerXYZ("lower plane, right edge, bottom corner:", lower_plane_right_bottom_corner_xyz);
            printCornerXYZ("lower plane, right edge, top corner:   ", lower_plane_right_top_corner_xyz);

            // LOWER BOX PLANE: BOTTOM EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //        6___7
            //        |   |
            //        |___|
            //        4   5

            //         ^ ^
            //         | |

            // Get corner cells of box
            QList<Eigen::Vector3d> lower_plane_bottom_left_cell_xyz = grid_->GetCell(imin_, jmin_, kmin_).corners();
            QList<Eigen::Vector3d> lower_plane_bottom_right_cell_xyz = grid_->GetCell(imax_, jmin_, kmin_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d lower_plane_bottom_left_corner_xyz = lower_plane_bottom_left_cell_xyz[4];
            Eigen::Vector3d lower_plane_bottom_right_corner_xyz = lower_plane_bottom_right_cell_xyz[5];
            // Print coordinates
            printCornerXYZ("lower plane, bottom edge, left corner: ", lower_plane_bottom_left_corner_xyz);
            printCornerXYZ("lower plane, bottom edge, right corner:", lower_plane_bottom_right_corner_xyz);

            // LOWER BOX PLANE: TOP EDGE
            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
            //         | |
            //         v v

            //        6___7
            //        |   |
            //        |___|
            //        4   5

            // Get corner cells of box
            QList<Eigen::Vector3d> lower_plane_top_left_cell_xyz = grid_->GetCell(imin_, jmax_, kmin_).corners();
            QList<Eigen::Vector3d> lower_plane_top_right_cell_xyz = grid_->GetCell(imax_, jmax_, kmin_).corners();
            // Get cell vertex amounting to true box corner
            Eigen::Vector3d lower_plane_top_left_corner_xyz = lower_plane_top_left_cell_xyz[6];
            Eigen::Vector3d lower_plane_top_right_corner_xyz = lower_plane_top_right_cell_xyz[7];
            // Print coordinates
            printCornerXYZ("lower plane, top edge, left corner:    ", lower_plane_top_left_corner_xyz);
            printCornerXYZ("lower plane, top edge, right corner:   ", lower_plane_top_right_corner_xyz);

            std::cout << std::setfill('-') << std::setw(80) << "-" << std::endl;
        }

        void ReservoirBoundary::printCornerXYZ(std::string str_out, Eigen::Vector3d vector_xyz) {
            std::cout << std::setfill(' ') << str_out ;
            for( int i=0, size=vector_xyz.size(); i < size; i++ )
            {
               std::cout << std::setw(8) << vector_xyz[i] << "\t";
            }
            std::cout << std::endl;
        }

        /* \brief Function getIndicesOfEdgeCells uses the limits
         * defining the box constraint to find the cells that constitute
         * the edges of the box
         */
        QList<int> ReservoirBoundary::getIndicesOfEdgeCells() {

            QList<int> box_edge_cells_;

            QList<int> upper_face_left_edge_;
            QList<int> upper_face_bottom_edge_;
            QList<int> upper_face_right_edge_;
            QList<int> upper_face_top_edge_;

            QList<Eigen::Vector3d> upper_face_left_edge_xyz;
            QList<Eigen::Vector3d> upper_face_left_edge_xyz_max_min;

            // UPPER CELL FACE: LEFT EDGE
            for (int j = jmin_; j <= jmax_; j++) {
                upper_face_left_edge_.append(grid_->GetCell(imin_, j, kmax_).global_index());
                upper_face_left_edge_xyz.append(grid_->GetCell(imin_, j, kmax_).corners());
            }

            // UPPER CELL FACE: BOTTOM EDGE
            for (int i = imin_; i <= imax_; i++) {
                upper_face_bottom_edge_.append(grid_->GetCell(i, jmin_, kmax_).global_index());
            }

            // UPPER CELL FACE: RIGHT EDGE
            for (int j = jmin_; j <= jmax_; j++) {
                upper_face_right_edge_.append(grid_->GetCell(imax_, j, kmax_).global_index());
            }

            // UPPER CELL FACE: TOP EDGE
            for (int i = imin_; i <= imax_; i++) {
                upper_face_top_edge_.append(grid_->GetCell(i, jmax_, kmax_).global_index());
            }

            // APPEND UPPER EDGE CELLS TO box_edge_cells_ LIST
            box_edge_cells_.append(upper_face_left_edge_);
            box_edge_cells_.append(upper_face_bottom_edge_);
            box_edge_cells_.append(upper_face_right_edge_);
            box_edge_cells_.append(upper_face_top_edge_);

            QList<int> lower_face_left_edge_;
            QList<int> lower_face_bottom_edge_;
            QList<int> lower_face_right_edge_;
            QList<int> lower_face_top_edge_;

            // LOWER CELL FACE: LEFT EDGE
            for (int j = jmin_; j <= jmax_; j++) {
               lower_face_left_edge_.append(grid_->GetCell(imin_, j, kmin_).global_index());
            }

            // LOWER CELL FACE: BOTTOM EDGE
            for (int i = imin_; i <= imax_; i++) {
               lower_face_bottom_edge_.append(grid_->GetCell(i, jmin_, kmin_).global_index());
            }

            // LOWER CELL FACE: RIGHT EDGE
            for (int j = jmin_; j <= jmax_; j++) {
               lower_face_right_edge_.append(grid_->GetCell(imax_, j, kmin_).global_index());
            }

            // LOWER CELL FACE: TOP EDGE
            for (int i = imin_; i <= imax_; i++) {
               lower_face_top_edge_.append(grid_->GetCell(i, jmax_, kmin_).global_index());
            }

            // APPEND LOWER EDGE CELLS TO box_edge_cells_ LIST
            box_edge_cells_.append(lower_face_left_edge_);
            box_edge_cells_.append(lower_face_bottom_edge_);
            box_edge_cells_.append(lower_face_right_edge_);
            box_edge_cells_.append(lower_face_top_edge_);

            return box_edge_cells_;
        }

        bool ReservoirBoundary::CaseSatisfiesConstraint(Case *c) {

            double heel_x_val = c->real_variables()[affected_well_.heel.x];
            double heel_y_val = c->real_variables()[affected_well_.heel.y];
            double heel_z_val = c->real_variables()[affected_well_.heel.z];

            double toe_x_val = c->real_variables()[affected_well_.toe.x];
            double toe_y_val = c->real_variables()[affected_well_.toe.y];
            double toe_z_val = c->real_variables()[affected_well_.toe.z];

            bool heel_feasible = false;
            bool toe_feasible = false;

            for (int ii=0; ii<index_list_.length(); ii++){
                if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
                        Eigen::Vector3d(heel_x_val, heel_y_val, heel_z_val))) {
                    heel_feasible = true;
                }
                if (grid_->GetCell(index_list_[ii]).EnvelopsPoint(
                        Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val))) {
                    toe_feasible = true;
                }
            }

            return heel_feasible && toe_feasible;
        }

        void ReservoirBoundary::SnapCaseToConstraints(Case *c) {

            double heel_x_val = c->real_variables()[affected_well_.heel.x];
            double heel_y_val = c->real_variables()[affected_well_.heel.y];
            double heel_z_val = c->real_variables()[affected_well_.heel.z];

            double toe_x_val = c->real_variables()[affected_well_.toe.x];
            double toe_y_val = c->real_variables()[affected_well_.toe.y];
            double toe_z_val = c->real_variables()[affected_well_.toe.z];

            Eigen::Vector3d projected_heel =
                    WellConstraintProjections::well_domain_constraint_indices(
                    Eigen::Vector3d(heel_x_val, heel_y_val, heel_z_val),
                    grid_,
                    index_list_
            );
            Eigen::Vector3d projected_toe =
                    WellConstraintProjections::well_domain_constraint_indices(
                    Eigen::Vector3d(toe_x_val, toe_y_val, toe_z_val),
                    grid_,
                    index_list_
            );

            c->set_real_variable_value(affected_well_.heel.x, projected_heel(0));
            c->set_real_variable_value(affected_well_.heel.y, projected_heel(1));
            c->set_real_variable_value(affected_well_.heel.z, projected_heel(2));

            c->set_real_variable_value(affected_well_.toe.x, projected_toe(0));
            c->set_real_variable_value(affected_well_.toe.y, projected_toe(1));
            c->set_real_variable_value(affected_well_.toe.z, projected_toe(2));
        }

        QList<int> ReservoirBoundary::getListOfCellIndices() {
             QList<int> index_list;

            for (int i = imin_; i <= imax_; i++){
                for (int j = jmin_; j <= jmax_; j++){
                    for (int k = kmin_; k <= kmax_; k++){
                        index_list.append(grid_->GetCell(i, j, k).global_index());
                    }
                }
            }
            return index_list;
        }

    }
}
