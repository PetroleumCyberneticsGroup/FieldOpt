/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

namespace Reservoir {
namespace Grid {

Cell::Cell(int global_index,
           IJKCoordinate ijk_index,
           float volume,
           Eigen::Vector3d center,
           std::vector<Eigen::Vector3d> corners)
{
    global_index_ = global_index;
    ijk_index_ = ijk_index;
    volume_ = volume;
    center_ = center;
    corners_ = corners;
    is_active_ = true;
    porosity_ = 0;
    permx_ = 0;
    permy_ = 0;
    permz_ = 0;
    initializeFaces();
}

bool Cell::Equals(const Cell *other) const
{
    return this->global_index() == other->global_index();
}

bool Cell::Equals(const Cell &other) const
{
    return this->global_index() == other.global_index();
}

bool Cell::EnvelopsPoint(Eigen::Vector3d point) {
    bool point_inside = true;
    for (Face face : faces_) {
        if ((point - face.corners[0]).dot(face.normal_vector) < 0)
            point_inside = false;
    }
    return point_inside;
}

void Cell::initializeFaces() {
    int face_indices_points[6][4] = {
        {0, 2, 1, 3},
        {4, 5, 6, 7},
        {0, 4, 2, 6},
        {1, 3, 5, 7},
        {0, 1, 4, 5},
        {2, 6, 3, 7}
    };

    for (int ii = 0; ii < 6; ii++) {
        Face face;
        face.corners.push_back(corners_[face_indices_points[ii][0]]);
        face.corners.push_back(corners_[face_indices_points[ii][1]]);
        face.corners.push_back(corners_[face_indices_points[ii][2]]);
        face.corners.push_back(corners_[face_indices_points[ii][3]]);

        face.normal_vector =
            (face.corners[2] - face.corners[0]).cross(
                face.corners[1] - face.corners[0]).normalized();

        faces_.push_back(face);
    }
}
void Cell::SetProperties(bool is_active, float porosity, float permx, float permy, float permz) {
    is_active_ = is_active;
    porosity_ = porosity;
    permx_ = permx;
    permy_ = permy;
    permz_ = permz;
}

}
}
