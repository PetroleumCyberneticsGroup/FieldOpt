#ifndef IJKCOORDINATE_H
#define IJKCOORDINATE_H

namespace Model {
namespace Reservoir {
namespace Grid {

/*!
 * \brief The IJKCoordinate class represents an integer-based (I,J,K) coordinate or index.
 * All IJK coordinates must be positive.
 */
class IJKCoordinate
{
public:
    IJKCoordinate(int i, int j, int k);

    int i() const { return i_; }
    int j() const { return j_; }
    int k() const { return k_;}

    bool Equals(const IJKCoordinate *other) const;

private:
    int i_;
    int j_;
    int k_;

};

}
}
}

#endif // IJKCOORDINATE_H
