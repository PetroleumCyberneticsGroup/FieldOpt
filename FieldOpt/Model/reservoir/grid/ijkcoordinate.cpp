#include "ijkcoordinate.h"
#include <assert.h>

namespace Model {
namespace Reservoir {
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

bool IJKCoordinate::Equals(const IJKCoordinate *other) const
{
    return this->i() == other->i() && this->j() == other->j() && this->k() == other->k();
}

}
}
}
