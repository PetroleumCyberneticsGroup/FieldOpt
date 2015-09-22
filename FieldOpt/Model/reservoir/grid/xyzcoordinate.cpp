#include "xyzcoordinate.h"
#include <cmath>

using std::abs;

namespace Model {
namespace Reservoir {
namespace Grid {

XYZCoordinate::XYZCoordinate(double x, double y, double z)
{
    x_ = x;
    y_ = y;
    z_ = z;
}

bool XYZCoordinate::Equals(XYZCoordinate *other, double epsilon)
{
    return abs(this->x() - other->x()) <= epsilon &&
            abs(this->y() - other->y()) <= epsilon &&
            abs(this->z() - other->z()) <= epsilon;
}

}
}
}
