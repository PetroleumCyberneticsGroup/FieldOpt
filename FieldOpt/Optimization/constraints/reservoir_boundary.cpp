#include "reservoir_boundary.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"

namespace Optimization {
    namespace Constraints {

        ReservoirBoundary::ReservoirBoundary(const Utilities::Settings::Optimizer::Constraint &settings,
                                             Model::Properties::VariablePropertyContainer *variables, Reservoir::Grid::Grid *grid)
        {
            imin_ = settings.imin;
            imax_ = settings.imax;
            jmin_ = settings.jmin;
            jmax_ = settings.jmax;
            kmin_ = settings.kmin;
            kmax_ = settings.kmax;
            grid_ = grid;
            index_list_ = getListOfCellIndices();
            affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));
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
                if( WellIndexCalculator::GeometryFunctions::is_point_inside_cell(
                        grid_->GetCell(index_list_[ii]),
                        QVector3D(heel_x_val, heel_y_val, heel_z_val)
                )){
                    heel_feasible = true;
                }
                if( WellIndexCalculator::GeometryFunctions::is_point_inside_cell(
                        grid_->GetCell(index_list_[ii]),
                        QVector3D(toe_x_val, toe_y_val, toe_z_val)
                )){
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

            Eigen::Vector3d projected_heel = WellIndexCalculator::WellConstraintProjections::well_domain_constraint_indices(
                    Eigen::Vector3d(heel_x_val, heel_y_val, heel_z_val),
                    grid_,
                    index_list_
            );
            Eigen::Vector3d projected_toe = WellIndexCalculator::WellConstraintProjections::well_domain_constraint_indices(
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
