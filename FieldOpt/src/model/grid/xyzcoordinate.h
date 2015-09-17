#ifndef XYZCOORDINATE_H
#define XYZCOORDINATE_H

namespace Model {
namespace Grid {

/*!
 * \brief The XYZCoordinate class represents a real-based coordinate (x, y, z).
 */
class XYZCoordinate
{
private:
    double x_;
    double y_;
    double z_;
public:
    XYZCoordinate(double x, double y, double z);

    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }
};

}
}

#endif // XYZCOORDINATE_H
