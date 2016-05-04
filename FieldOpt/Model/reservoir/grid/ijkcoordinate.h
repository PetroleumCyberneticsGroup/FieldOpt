#ifndef IJKCOORDINATE_H
#define IJKCOORDINATE_H

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The IJKCoordinate class represents an integer-based (I,J,K) coordinate or index.
 * IJK coordinate objects are immutable.
 */
class IJKCoordinate
{
public:
    IJKCoordinate(){}
    IJKCoordinate(int i, int j, int k);


    int i() const { return i_; }
    int j() const { return j_; }
    int k() const { return k_;}

    bool Equals(const IJKCoordinate *other) const;

    /*!
     * \brief Add Returns a new coordinate which is the sum of this coordinate and another coordinate.
     */
    IJKCoordinate *Add(const IJKCoordinate *other) const;

private:
    int i_;
    int j_;
    int k_;
};

}
}
}

#endif // IJKCOORDINATE_H
