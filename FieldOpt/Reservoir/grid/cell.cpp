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
            center_ = Eigen::Vector3d(center.x(), center.y(), center.z());
            for (XYZCoordinate corner : corners)
                corners_.append(Eigen::Vector3d(corner.x(), corner.y(), corner.z()));
        }

        bool Cell::Equals(const Cell *other) const
        {
            return this->global_index() == other->global_index();
        }

        bool Cell::Equals(const Cell &other) const
        {
            return this->global_index() == other.global_index();
        }

    }
}
