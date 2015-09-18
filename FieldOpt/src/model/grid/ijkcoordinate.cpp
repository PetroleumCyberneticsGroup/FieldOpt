#include "ijkcoordinate.h"
#include <assert.h>

namespace Model {
namespace Grid {

IJKCoordinate::IJKCoordinate(int i, int j, int k)
{
    assert(i >= 0);
    assert(j >= 0);
    assert(k >= 0);
    i_ = i;
    j_ = j;
    k_ = k;
}

bool IJKCoordinate::operator==(const IJKCoordinate &other)
{
    return i_ == other.i() && j_ == other.j() && k_ == other.k();
}

bool IJKCoordinate::operator!=(const IJKCoordinate &other)
{
    return i_ != other.i() || j_ != other.j() || k_ != other.k();
}

}
}
