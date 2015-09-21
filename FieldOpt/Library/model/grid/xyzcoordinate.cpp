#include "xyzcoordinate.h"
#include <cmath>

using std::abs;

Model::Grid::XYZCoordinate::XYZCoordinate(double x, double y, double z)
{
    x_ = x;
    y_ = y;
    z_ = z;
}

bool Model::Grid::XYZCoordinate::Equals(Model::Grid::XYZCoordinate *other, double epsilon)
{
    return abs(this->x() - other->x()) <= epsilon &&
            abs(this->y() - other->y()) <= epsilon &&
            abs(this->z() - other->z()) <= epsilon;
}

