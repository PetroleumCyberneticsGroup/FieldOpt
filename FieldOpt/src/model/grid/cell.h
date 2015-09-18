#ifndef CELL_H
#define CELL_H

#include <QList>
#include "ijkcoordinate.h"
#include "xyzcoordinate.h"

namespace Model {
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

    int global_index() const { return global_index_; }
    IJKCoordinate *ijk_index() const { return ijk_index_; }
    double volume() const { return volume_; }
    XYZCoordinate *center() const { return center_; }
    QList<XYZCoordinate*> corners() const { return corners_; }
};

}
}

#endif // CELL_H
