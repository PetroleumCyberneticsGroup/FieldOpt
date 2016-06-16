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
                face.corners.append(corners_[face_indices_points[ii][0]]);
                face.corners.append(corners_[face_indices_points[ii][1]]);
                face.corners.append(corners_[face_indices_points[ii][2]]);
                face.corners.append(corners_[face_indices_points[ii][3]]);
                face.normal_vector = (face.corners[2] - face.corners[0]).cross(face.corners[1] - face.corners[0]);
                faces_.append(face);
            }
        }


    }
}
