#include "cell.h"

namespace Reservoir {
    namespace Grid {


        Cell::Cell(int global_index, IJKCoordinate ijk_index,
                   double volume, double poro, double permx, double permy, double permz,
                   Eigen::Vector3d center, QList<Eigen::Vector3d> corners)
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
            initializePlanes();
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
            for (Plane plane : planes_) {
                if ((point - plane.corners[0]).dot(plane.normal_vector) < 0)
                    point_inside = false;
            }
            return point_inside;
        }

        void Cell::initializePlanes() {
            int points[6][3] = {
                    {0, 2, 1},
                    {4, 5, 6},
                    {0, 4, 2},
                    {1, 3, 5},
                    {0, 1, 4},
                    {2, 6, 3}
            };

            for (int ii = 0; ii < 6; ii++) {
                Plane plane;
                plane.corners[0] = corners_[points[ii][0]];
                plane.corners[1] = corners_[points[ii][1]];
                plane.corners[2] = corners_[points[ii][2]];
                plane.normal_vector = (plane.corners[2] - plane.corners[0]).cross(plane.corners[1] - plane.corners[0]);
                planes_[ii] = plane;
            }
        }


    }
}
