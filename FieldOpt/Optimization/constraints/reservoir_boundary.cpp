/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include "reservoir_boundary.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"
#include <iomanip>
#include "Utilities/math.hpp"

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
    penalty_weight_ = settings.penalty_weight;

    index_list_ = getListOfCellIndices();
    if (variables->GetWellSplineVariables(settings.well).size() > 0)
        affected_well_ = initializeWell(variables->GetWellSplineVariables(settings.well));
    else
        affected_well_ = initializeWell(variables->GetPolarSplineVariables(settings.well));

    // QList with indices of box edge cells
    index_list_edge_ = getIndicesOfEdgeCells();

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
    std::vector<Eigen::Vector3d> upper_plane_left_bottom_cell_xyz = grid_->GetCell(imin_, jmin_, kmax_).corners();
    std::vector<Eigen::Vector3d> upper_plane_left_top_cell_xyz = grid_->GetCell(imin_, jmax_, kmax_).corners();
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
    std::vector<Eigen::Vector3d> upper_plane_right_bottom_cell_xyz = grid_->GetCell(imax_, jmin_, kmax_).corners();
    std::vector<Eigen::Vector3d> upper_plane_right_top_cell_xyz = grid_->GetCell(imax_, jmax_, kmax_).corners();
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
    std::vector<Eigen::Vector3d> upper_plane_bottom_left_cell_xyz = grid_->GetCell(imin_, jmin_, kmax_).corners();
    std::vector<Eigen::Vector3d> upper_plane_bottom_right_cell_xyz = grid_->GetCell(imax_, jmin_, kmax_).corners();
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
    std::vector<Eigen::Vector3d> upper_plane_top_left_cell_xyz = grid_->GetCell(imin_, jmax_, kmax_).corners();
    std::vector<Eigen::Vector3d> upper_plane_top_right_cell_xyz = grid_->GetCell(imax_, jmax_, kmax_).corners();
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
    std::vector<Eigen::Vector3d> lower_plane_left_bottom_cell_xyz = grid_->GetCell(imin_, jmin_, kmin_).corners();
    std::vector<Eigen::Vector3d> lower_plane_left_top_cell_xyz = grid_->GetCell(imin_, jmax_, kmin_).corners();
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
    std::vector<Eigen::Vector3d> lower_plane_right_bottom_cell_xyz = grid_->GetCell(imax_, jmin_, kmin_).corners();
    std::vector<Eigen::Vector3d> lower_plane_right_top_cell_xyz = grid_->GetCell(imax_, jmax_, kmin_).corners();
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
    std::vector<Eigen::Vector3d> lower_plane_bottom_left_cell_xyz = grid_->GetCell(imin_, jmin_, kmin_).corners();
    std::vector<Eigen::Vector3d> lower_plane_bottom_right_cell_xyz = grid_->GetCell(imax_, jmin_, kmin_).corners();
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
    std::vector<Eigen::Vector3d> lower_plane_top_left_cell_xyz = grid_->GetCell(imin_, jmax_, kmin_).corners();
    std::vector<Eigen::Vector3d> lower_plane_top_right_cell_xyz = grid_->GetCell(imax_, jmax_, kmin_).corners();
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
 * defining the box constraint to find the cells that make
 * up the edges of the box
 */
QList<int> ReservoirBoundary::getIndicesOfEdgeCells() {
    QList<int> box_edge_cells_;

    QList<int> upper_face_left_edge_;
    QList<int> upper_face_bottom_edge_;
    QList<int> upper_face_right_edge_;
    QList<int> upper_face_top_edge_;

    QList<Eigen::Vector3d> upper_face_left_edge_xyz;
    QList<Eigen::Vector3d> upper_face_left_edge_xyz_max_min;

    // Testing
//    std::vector<Eigen::Matrix<double,3,1,0,3,1>> upper_face_left_edge_xyz;
//    std::vector<Eigen::Matrix<double,3,1,0,3,1>> upper_face_left_edge_xyz_max_min;

    // UPPER CELL FACE: LEFT EDGE
    for (int j = jmin_; j <= jmax_; j++) {
        upper_face_left_edge_.append(grid_->GetCell(imin_, j, kmax_).global_index());
//        upper_face_left_edge_xyz.append(grid_->GetCell(imin_, j, kmax_).corners());
        // Testing
        // upper_face_left_edge_xyz.push_back(grid_->GetCell(imin_, j, kmax_).corners());
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
Eigen::VectorXd ReservoirBoundary::GetLowerBounds(QList<QUuid> id_vector) const {
    auto cell_min = grid_->GetCell(imin_, jmin_, kmin_);
    auto cell_max = grid_->GetCell(imax_, jmax_, kmax_);
    double xmin, ymin, zmin;
    xmin = std::min(cell_max.center().x(), cell_min.center().x());
    ymin = std::min(cell_max.center().y(), cell_min.center().y());
    zmin = std::min(cell_max.corners()[0].z(), cell_max.corners()[7].z());
    zmin = std::min(zmin, cell_min.corners()[0].z());
    zmin = std::min(zmin, cell_min.corners()[7].z());


    Eigen::VectorXd lbounds(id_vector.size());
    lbounds.fill(0);

    int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
    int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
    int ind_heel_z = id_vector.indexOf(affected_well_.heel.z);
    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
    lbounds(ind_heel_x) = xmin;
    lbounds(ind_toe_x) = xmin;
    lbounds(ind_heel_y) = ymin;
    lbounds(ind_toe_y) = ymin;
    lbounds(ind_heel_z) = zmin;
    lbounds(ind_toe_z) = zmin;
    return lbounds;
}
Eigen::VectorXd ReservoirBoundary::GetUpperBounds(QList<QUuid> id_vector) const {
    auto cell_min = grid_->GetCell(imin_, jmin_, kmin_);
    auto cell_max = grid_->GetCell(imax_, jmax_, kmax_);
    double xmax, ymax, zmax;
    xmax = std::max(cell_max.center().x(), cell_min.center().x());
    ymax = std::max(cell_max.center().y(), cell_min.center().y());
    zmax = std::max(cell_max.corners()[0].z(), cell_max.corners()[7].z());
    zmax = std::max(zmax, cell_min.corners()[0].z());
    zmax = std::max(zmax, cell_min.corners()[7].z());

    Eigen::VectorXd ubounds(id_vector.size());
    ubounds.fill(0);

    int ind_heel_x = id_vector.indexOf(affected_well_.heel.x);
    int ind_heel_y = id_vector.indexOf(affected_well_.heel.y);
    int ind_heel_z = id_vector.indexOf(affected_well_.heel.z);
    int ind_toe_x = id_vector.indexOf(affected_well_.toe.x);
    int ind_toe_y = id_vector.indexOf(affected_well_.toe.y);
    int ind_toe_z = id_vector.indexOf(affected_well_.toe.z);
    ubounds(ind_heel_x) = xmax;
    ubounds(ind_toe_x) = xmax;
    ubounds(ind_heel_y) = ymax;
    ubounds(ind_toe_y) = ymax;
    ubounds(ind_heel_z) = zmax;
    ubounds(ind_toe_z) = zmax;
    return ubounds;
}
void ReservoirBoundary::InitializeNormalizer(QList<Case *> cases) {
    normalizer_.set_max(1.0L);
    normalizer_.set_midpoint(0.0L);
    normalizer_.set_steepness(1.0L);
}
double ReservoirBoundary::Penalty(Case *c) {
    if (CaseSatisfiesConstraint(c))
        return 0.0;
    else
        return INFINITY;
}
long double ReservoirBoundary::PenaltyNormalized(Case *c) {
    if (CaseSatisfiesConstraint(c))
        return 0.0L;
    else
        return 1.0L;
}

}
}
