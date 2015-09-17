#include "cell.h"

namespace Model {
namespace Grid {

Cell::Cell()
{
    global_index_ = -1;
    ijk_index_ = new IJKCoordinate(0, 0, 0);
    volume_ = -1;
    center_ = new XYZCoordinate(-1, -1, -1);
    corners_ = QList<XYZCoordinate*>();
    corners_.append(new XYZCoordinate(-1,-1,-1));
}

Cell::Cell(int global_index, IJKCoordinate* ijk_index, double volume, XYZCoordinate* center, QList<XYZCoordinate *> corners)
{
    global_index_ = global_index;
    ijk_index_ = ijk_index;
    volume_ = volume;
    center_ = center;
    corners_ = corners;
}

}
}
