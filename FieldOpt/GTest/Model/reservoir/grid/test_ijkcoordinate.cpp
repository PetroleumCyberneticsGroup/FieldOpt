#include <gtest/gtest.h>
#include "Model/reservoir/grid/ijkcoordinate.h"

using namespace Model::Reservoir::Grid;

namespace {

class IJKCoordinateTest : public ::testing::Test {
protected:
    IJKCoordinateTest () { }
    virtual ~IJKCoordinateTest () { }
};

TEST_F(IJKCoordinateTest , Equality) {
    IJKCoordinate *ijk_1 = new IJKCoordinate(1,2,3);
    IJKCoordinate *ijk_2 = new IJKCoordinate(1,2,3);
    IJKCoordinate *ijk_3 = new IJKCoordinate(2,2,3);
    EXPECT_TRUE(ijk_1->Equals(ijk_2));
    EXPECT_FALSE(ijk_1->Equals(ijk_3));
}

}

