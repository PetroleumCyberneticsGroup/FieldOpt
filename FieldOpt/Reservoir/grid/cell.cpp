#include "cell.h"

namespace Reservoir {
    namespace Grid {


        Cell::Cell(int global_index, IJKCoordinate ijk_index,
                   double volume, double poro, double permx, double permy, double permz,
                   XYZCoordinate center, QList<XYZCoordinate> corners)
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
        }

        bool Cell::Equals(const Cell *other) const
        {
            return this->global_index() == other->global_index();
        }

        bool Cell::Equals(const Cell &other) const
        {
            return this->global_index() == other.global_index();
        }

        QList<Eigen::Vector3d> Cell::corners_eigen() const {
            QList<Eigen::Vector3d> corners;
            for (auto corner : corners_) {
                corners.append(Eigen::Vector3d(corner.x(), corner.y(), corner.z()));
            }
            return corners;
        }


    }
}
