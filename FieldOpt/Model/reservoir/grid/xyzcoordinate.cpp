#include "xyzcoordinate.h"
#include <cmath>

using std::abs;

namespace Model {
namespace Reservoir {
namespace Grid {

XYZCoordinate::XYZCoordinate(double x, double y, double z)
    : x_(x), y_(y), z_(z)
{
}

bool XYZCoordinate::Equals(const XYZCoordinate *other, double epsilon) const
{
    return abs(this->x() - other->x()) <= epsilon &&
            abs(this->y() - other->y()) <= epsilon &&
            abs(this->z() - other->z()) <= epsilon;
}

bool XYZCoordinate::Equals(const XYZCoordinate &other, double epsilon) const
{
    return abs(this->x() - other.x()) <= epsilon &&
            abs(this->y() - other.y()) <= epsilon &&
            abs(this->z() - other.z()) <= epsilon;
}

XYZCoordinate *XYZCoordinate::Add(const XYZCoordinate *other) const
{
    return new XYZCoordinate(
                this->x() + other->x(),
                this->y() + other->y(),
                this->z() + other->z());
}

}
}
}
