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

#ifndef CELL_H
#define CELL_H

#include <Eigen/Dense>
#include "ijkcoordinate.h"

namespace Reservoir {
    namespace Grid {

        /*!
         * \brief The Cell class describes a cell in a grid, including it's geometry and static properties
         * like porosity and permeability.
         */
        class Cell
        {
        public:
            Cell(){};
            Cell(int global_index, IJKCoordinate ijk_index,
                 double volume, double poro, double permx, double permy, double permz,
                 Eigen::Vector3d center, std::vector<Eigen::Vector3d> corners);

            /*!
             * \brief global_index Gets the cells global index in its parent grid.
             */
            int global_index() const { return global_index_; }

            /*!
             * \brief ijk_index Gets the cells (i, j, k) index in its parent grid.
             */
            IJKCoordinate ijk_index() const { return ijk_index_; }

            /*!
             * \brief volume Gets the cells volume.
             */
            double volume() const { return volume_; }

            /*!
             * \brief porosity Gets the cell's porosity.
             */
            double porosity() const { return porosity_; }

            /*!
             * \brief porosity Gets the cell's x-permeability.
             */
            double permx() const { return permx_; }

            /*!
             * \brief porosity Gets the cell's y-permeability.
             */
            double permy() const { return permy_; }

            /*!
             * \brief porosity Gets the cell's z-permeability.
             */
            double permz() const { return permz_; }


            /*!
             * \brief center Gets the (x, y, z) position of the cells center.
             */
            Eigen::Vector3d center() const { return center_; }

            /*!
             * \brief corners Gets the (x, y, z) coordinates of the cells 8 corners.
             *
             * The first four elements represent the corners in the top layer,
             * the four last represent the corners in the bottom layer:
             *
             * Top:    Bottom:
             * 2---3    6---7
             * |   |    |   |
             * 0---1    4---5
             *
             */
            std::vector<Eigen::Vector3d> corners() const { return corners_; }

            /*!
             * \brief Equals Check if the global indices of the two cells being compared are equal.
             */
            bool Equals(const Cell *other) const;
            bool Equals(const Cell &other) const;

            /*!
             * \brief Check whether a point is inside or on the boundary of this cell.
             */
            bool EnvelopsPoint(Eigen::Vector3d point);

            /*!
             * \todo Hilmar should probably be the one to document this thing.
             */
            struct Face {
                std::vector<Eigen::Vector3d> corners;
                Eigen::Vector3d normal_vector;

                /*!
                 * \brief point_on_same_side returns true if point is on the same side of this plane,
                 * true if it is in the plane, and false if it's on the other side.
                 *
                 * In the function, a dot product helps us determine if the angle between the two
                 * vectors is below (positive answer), at (zero answer) or above
                 * (negative answer) 90 degrees. Essentially telling us which side
                 * of a plane the point is.
                 *
                 * \param point The point to be checked.
                 * \param slack A slack factor.
                 * \return True if the point is on the same side as the normal vector or in the plane; otherwise false.
                 */
                bool point_on_same_side(const Eigen::Vector3d &point, const double slack) {
                    return (point - corners[0]).dot(normal_vector) >= 0.0 - slack;
                }

                /*!
                 * Find the point of inntersection between a line and this plane.
                 * \param p0 Point defining one end of the line.
                 * \param p1 Point defining other end of the line.
                 * \return The point of intersection.
                 */
                Eigen::Vector3d intersection_with_line(const Eigen::Vector3d &p0, const Eigen::Vector3d &p1) {
                    Eigen::Vector3d line_vector = (p1 - p0).normalized();
                    auto w = p0 - corners[0];
                    double s = normal_vector.dot(-w) / normal_vector.dot(line_vector);
                    return p0 + s*line_vector;
                }
            };

            std::vector<Face> faces() const { return faces_; }


        private:
            int global_index_;
            IJKCoordinate ijk_index_;
            double volume_;
            Eigen::Vector3d center_;
            std::vector<Eigen::Vector3d> corners_;
            double porosity_;
            double permx_;
            double permy_;
            double permz_;
            std::vector<Face> faces_;

            /*!
             * \brief Populates the faces_ field.
             *
             * Generates a double array with the numbers of 3 corners from each of the 6 faces of this cell that
             * will be used to create a normal vector for each face.

             * \todo Clarify this comment.

             * \return double list of corner numbers for each face
             */
            void initializeFaces();
        };

    }
}

#endif // CELL_H
