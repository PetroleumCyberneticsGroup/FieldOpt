/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016 - 2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   
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

namespace Reservoir 
{
    namespace Grid 
    {
        Cell::Cell(int global_index, IJKCoordinate ijk_index,
                   double volume, double poro, double permx, double permy, double permz,
                   Eigen::Vector3d center, std::vector<Eigen::Vector3d> corners)
        {
            global_index_ = global_index;
            ijk_index_ = ijk_index;
            volume_ = volume;
            porosity_ = poro;
            permx_ = permx;
            permy_ = permy;
            permz_ = permz;
            center_ = center;
            corners_ = corners;
            
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

        constexpr const std::array<std::array<int,4>, 6> Cell::_faces_definition_earth_pointing_z;
        constexpr const std::array<std::array<int,4>, 6> Cell::_faces_definition_ski_pointing_z;
        
        void Cell::initializeFaces() 
        {
        	// The code assumes that the corners of the cell are given in the following order 
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
            //     Above and below here mean position relative to eachother, i.e the "Above" layer sits above the "Below" layer 
        	// with respect to the center of the Earth (Below is closer to the center of the Earth than Above)
        	//
        	// However the grids can be left-handed, i.e. the direction of increasing z will
        	// point down towards the center of the Earth or right-handed, 
        	// i.e. the direction of increasing z will be away from the center of the Earth.
        	
        	// Make sure there is a good definition of inside and outside of the cell.
        	
        	std::array<std::array<int,4>, 6> face_indices_points;
        	if (corners_[0].z() < corners_[4].z()) // z-axis grows towards the center of the Earth - left-handed
        	{
        		face_indices_points = _faces_definition_earth_pointing_z; 
        	}
        	else // z-axis grows towards the ski - right-handed
        	{
				face_indices_points = _faces_definition_ski_pointing_z;
        	}

//            int face_indices_points[6][4] = 
//        	{
//				 {0, 2, 1, 3},
//				 {4, 5, 6, 7},
//				 {0, 4, 2, 6},
//				 {1, 3, 5, 7},
//				 {0, 1, 4, 5},
//				 {2, 6, 3, 7}
//            };

//            int face_indices_points[6][4] = 
//            {
//				{2, 0, 3, 1},
//				{6, 7, 4, 5},
//				{2, 6, 0, 4},
//				{3, 1, 7, 5},
//				{2, 3, 6, 7},
//				{0, 4, 1, 5}
//            };
	    
            for (int ii = 0; ii < 6; ii++) 
            {               
            	Face face;
                face.corners.push_back(corners_[face_indices_points[ii][0]]);
                face.corners.push_back(corners_[face_indices_points[ii][1]]);
                face.corners.push_back(corners_[face_indices_points[ii][2]]);
                face.corners.push_back(corners_[face_indices_points[ii][3]]);
                face.normal_vector = (face.corners[2] - face.corners[0]).cross(face.corners[1] - face.corners[0]).normalized();
                faces_.push_back(face);
            }
        }
    }
}
