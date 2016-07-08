#include "reservoir_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"

namespace Optimization {
    namespace Constraints {

        ReservoirBoundary::ReservoirBoundary(
                const Utilities::Settings::Optimizer::Constraint &settings,
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
            index_list_edge_ = getListOfBoxEdgeCellIndices();
            affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));
        }

        /* \brief Function getListOfBoxEdgeCellIndices uses the limits
         * defining the box constraint to find the cells that constitute
         * the edges of the box
         */
        QList<int> ReservoirBoundary::getListOfBoxEdgeCellIndices() {
            QList<int> box_edge_cells_;

            QList<int> upper_face_left_edge_;
            QList<int> upper_face_bottom_edge_;
            QList<int> upper_face_right_edge_;
            QList<int> upper_face_top_edge_;

            // UPPER CELL FACE: LEFT EDGE
            for (int j = jmin_; j <= jmax_; j++) {
                upper_face_left_edge_.append(grid_->GetCell(imin_, j, kmax_).global_index());
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
