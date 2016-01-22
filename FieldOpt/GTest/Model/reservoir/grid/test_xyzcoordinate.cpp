#include <gtest/gtest.h>
#include "Model/reservoir/grid/xyzcoordinate.h"

using namespace Model::Reservoir::Grid;

namespace {

class XYZCoordinateTest : public ::testing::Test {
protected:
    XYZCoordinateTest () { }
    virtual ~XYZCoordinateTest () { }
};

TEST_F(XYZCoordinateTest , Equality) {
    XYZCoordinate xyz_1 = XYZCoordinate(1.0,2.0,3.0);
    XYZCoordinate xyz_2 = XYZCoordinate(1.0,2.0,3.0);
    XYZCoordinate xyz_3 = XYZCoordinate(1.0,2.0,3.05);
    XYZCoordinate xyz_4 = XYZCoordinate(1.0,2.0,4.0);
    EXPECT_TRUE(xyz_1.Equals(&xyz_2));  // Should match with tolerance 0.0
    EXPECT_FALSE(xyz_1.Equals(&xyz_3)); // Should not match with tolerance 0.0
    EXPECT_FALSE(xyz_1.Equals(&xyz_3, 0.04)); // Should not match with tolerance 0.4
    EXPECT_TRUE(xyz_1.Equals(&xyz_3, 0.1)); // Should match with tolerance 0.1
    EXPECT_FALSE(xyz_1.Equals(&xyz_4, 0.1)); // Should not match with tolerance 0.1
}


TEST_F(XYZCoordinateTest, Addition) {
    XYZCoordinate *xyz_1 = new XYZCoordinate(1,1,1);
    XYZCoordinate *xyz_2 = new XYZCoordinate(3,4,5);
    XYZCoordinate *xyz_3 = new XYZCoordinate(4,5,6);
    XYZCoordinate *xyz_sum = xyz_1->Add(xyz_2);
    EXPECT_TRUE(xyz_sum->Equals(xyz_3));
}

}
