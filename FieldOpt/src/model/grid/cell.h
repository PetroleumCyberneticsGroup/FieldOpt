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
};

}
}

#endif // CELL_H
