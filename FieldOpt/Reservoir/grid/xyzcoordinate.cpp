#include "xyzcoordinate.h"
#include <cmath>

using std::abs;

namespace Reservoir {
    namespace Grid {

        XYZCoordinate::XYZCoordinate(double x, double y, double z)
                : x_(x), y_(y), z_(z)
        {
        }

        XYZCoordinate::XYZCoordinate(QVector3D point) {
            x_ = point.x();
            y_ = point.y();
            z_ = point.z();
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


        QVector3D XYZCoordinate::toQvec() const {
            return QVector3D(x_, y_, z_);
        }

        QVector3D XYZCoordinate::vectorTo(const XYZCoordinate other) const {
            return other.toQvec() - this->toQvec();
        }

        Eigen::Vector3d XYZCoordinate::vectorTo_eigen(const XYZCoordinate other) const {
            return Eigen::Vector3d(other.x(), other.y(), other.z()) - Eigen::Vector3d(this->x(), this->y(), this->z());
        }

        Eigen::Vector3d XYZCoordinate::toEigenVec() const {
            return Eigen::Vector3d(x_, y_, z_);
        }


    }
}
