#include <gtest/gtest.h>
#include "model/grid/ijkcoordinate.h"

using namespace Model::Grid;

namespace {

class IJKCoordinateTest : public ::testing::Test {
protected:
    IJKCoordinateTest () { }
    virtual ~IJKCoordinateTest () { }
};

TEST_F(IJKCoordinateTest , Equality) {
    IJKCoordinate ijk_1 = IJKCoordinate(1,2,3);
    IJKCoordinate ijk_2 = IJKCoordinate(1,2,3);
    IJKCoordinate ijk_3 = IJKCoordinate(2,2,3);
    EXPECT_TRUE(ijk_1 == ijk_2);
    EXPECT_FALSE(ijk_1 == ijk_3);
}

TEST_F(IJKCoordinateTest, Inequality) {
    IJKCoordinate ijk_1 = IJKCoordinate(1,2,3);
    IJKCoordinate ijk_2 = IJKCoordinate(1,2,3);
    IJKCoordinate ijk_3 = IJKCoordinate(2,2,3);
    EXPECT_TRUE(ijk_1 != ijk_3);
    EXPECT_FALSE(ijk_1 != ijk_2);
}

}

