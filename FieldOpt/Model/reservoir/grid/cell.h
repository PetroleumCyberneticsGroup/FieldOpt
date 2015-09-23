#ifndef CELL_H
#define CELL_H

#include <QList>
#include "ijkcoordinate.h"
#include "xyzcoordinate.h"

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The Cell class describes a cell in a grid.
 */
class Cell
{
private:
    int global_index_;
    IJKCoordinate* ijk_index_;
    double volume_;
    XYZCoordinate* center_;
    QList<XYZCoordinate*> corners_;

public:
    Cell(int global_index, IJKCoordinate* ijk_index, double volume, XYZCoordinate* center, QList<XYZCoordinate*> corners);

    /*!
     * \brief global_index Gets the cells global index in its parent grid.
     */
    int global_index() const { return global_index_; }

    /*!
     * \brief ijk_index Gets the cells (i, j, k) index in its parent grid.
     */
    IJKCoordinate *ijk_index() const { return ijk_index_; }

    /*!
     * \brief volume Gets the cells volume.
     */
    double volume() const { return volume_; }

    /*!
     * \brief center Gets the (x, y, z) position of the cells center.
     */
    XYZCoordinate *center() const { return center_; }

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
    QList<XYZCoordinate*> corners() const { return corners_; }

    /*!
     * \brief Equals Check if the global indices of the two cells being compared are equal.
     */
    bool Equals(const Cell *other) const;
};

}
}
}

#endif // CELL_H
