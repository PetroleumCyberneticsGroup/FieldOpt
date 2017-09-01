/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Mathias Bellout (2017) <mathias.bellout@ntnu.no, chakibbb@gmail.com>

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

#include "cell.h"
#include <iostream>

namespace Reservoir {
namespace Grid {

using namespace std;

Cell::Cell(int global_index, IJKCoordinate ijk_index,
           double volume, vector<double> poro_in,
           vector<double> permx_in, vector<double> permy_in, vector<double> permz_in,
           Eigen::Vector3d center,
           vector<Eigen::Vector3d> corners,
           int faces_permutation_index,
           bool active_matrix, bool active_fracture, int k_fracture_index)
{
    global_index_ = global_index;
    ijk_index_ = ijk_index;
    k_fracture_index_ = k_fracture_index;
    volume_ = volume;
    porosity_ = poro_in;
    permx_ = permx_in;
    permy_ = permy_in;
    permz_ = permz_in;
    center_ = center;
    corners_ = corners;
    is_active_matrix_ = active_matrix;
    is_active_fracture_ = active_fracture;

    initializeFaces(faces_permutation_index);
}

bool Cell::Equals(const Cell *other) const
{
    return this->global_index() == other->global_index();
}

bool Cell::Equals(const Cell &other) const
{
    return this->global_index() == other.global_index();
}

bool Cell::EnvelopsPoint(Eigen::Vector3d point)
{
    bool point_inside = true;
    for (Face face : faces_)
    {
        double dot_prod = (point - face.corners[0]).dot(face.normal_vector);
        if ( dot_prod < 0)
        {
            point_inside = false;
            break;
        }
    }

    return point_inside;
}

vector<array<array<int,4>, 6>> Cell::MakeFacesPerturbation()
{
    vector<array<array<int,4>, 6>> v;
    v.push_back(
        array<array<int,4>,6>{{
                                  {0, 2, 1, 3},
                                  {4, 5, 6, 7},
                                  {0, 4, 2, 6},
                                  {1, 3, 5, 7},
                                  {0, 1, 4, 5},
                                  {2, 6, 3, 7}}
        });

    v.push_back(
        array<array<int,4>,6>{{
                                  {2, 0, 3, 1},
                                  {6, 7, 4, 5},
                                  {2, 6, 0, 4},
                                  {3, 1, 7, 5},
                                  {2, 3, 6, 7},  // actual diff from indexes above
                                  {0, 4, 1, 5}}  // actual diff from indexes above
        });

    return v;
}

vector<array<array<int,4>, 6>> Cell::faces_indices_permutation = MakeFacesPerturbation();

string Cell::to_string() const {
    std::stringstream str;
    str << "Cell " << global_index_ << ijk_index().to_string();
    return str.str();
}

void Cell::initializeFaces(int faces_permutation_index)
{
    // The code assumes the corners of the cell are given in the following order
    //
    //     Above:
    //     2---3
    //     |   |
    //     0---1
    //
    //     Below:
    //     6---7
    //     |   |
    //     4---5
    //
    // Above and below here refer to the relative position of the two cell faces
    // with normal vectors less than 90 degrees relative to the axis from the center
    // of the Earth. Here "Above" refers to the face that is farther away from the
    // Earth center, while "Below" refers to the face closer to the center of the
    // Earth.
    //
    // \todo double-check above assumption regarding which cell faces
    // that have which index ordering; either way, the definitions of
    // faces_definition_earth_pointing_z_ and
    // faces_definition_sky_pointing_z_ are correct relative to each
    // other, so this provides the necessary switching between pointing
    // down or up z-axis
    //
    // Reservoir grids can be either left- or right-handed. If left-handed, the
    // direction of increasing z will point down towards the center of the Earth,
    // while if right-handed, the direction of increasing z will be away from the
    // center of the Earth.

    // Here we determine whether the cell is left- or right-handed, which will
    // later determine whether the vertex indices sets, (0-1-2-3) and (4-5-6-7),
    // are assigned as "Above" or "Below" cell faces.

    // \todo To avoid having to perfom this check for every grid cell:
    // move this check to grid constructor, e.g., there check only one
    // cell to determine whether the entire grid is left or right-handed.
    // Any cell object can later make use of this information by collecting
    // it from the grid object

    // Make sure there is a good definition of inside and outside of the cell.

    //  std::array<std::array<int,4>, 6> face_indices_points;
//
//  if (corners_[0].z() < corners_[4].z()) {
//    // z-axis grows towards Earth's center (left-handed)
//    face_indices_points = faces_definition_earth_pointing_z_;
//    face_indices_points = faces_definition_sky_pointing_z_;
//    std::cout << "\\/" << std::endl;
//  }
//  else {
//    // z-axis grows towards the sky - right-handed
//    face_indices_points = faces_definition_sky_pointing_z_;
//    std::cout << "^" << std::endl;
//  }

    for (int ii = 0; ii < 6; ii++) {
        Face face;
        face.corners.push_back(corners_[faces_indices_permutation[faces_permutation_index][ii][0]]);
        face.corners.push_back(corners_[faces_indices_permutation[faces_permutation_index][ii][1]]);
        face.corners.push_back(corners_[faces_indices_permutation[faces_permutation_index][ii][2]]);
        face.corners.push_back(corners_[faces_indices_permutation[faces_permutation_index][ii][3]]);

        face.normal_vector = (
            face.corners[2] - face.corners[0]).cross(
            face.corners[1] - face.corners[0]).normalized();
        faces_.push_back(face);
    }
}
}
}
