
#include <gtest/gtest.h>
#include <QList>
#include <QVector3D>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Reservoir/grid/eclgrid.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "Utilities/file_handling/filehandling.h"

using namespace Reservoir::Grid;

namespace {

class GeometryFunctionsTest : public ::testing::Test {
protected:
    GeometryFunctionsTest() {
        grid_ = new ECLGrid(file_path_);
    }

    virtual ~GeometryFunctionsTest() {
        delete grid_;
    }

    virtual void SetUp() {
        corner_ = Reservoir::Grid::XYZCoordinate(4, 4, 0);
        vector_point_ = QVector3D(4,4,0);
    }

    virtual void TearDown() { }

    Reservoir::Grid::XYZCoordinate corner_;
    QVector3D vector_point_;

    Grid* grid_;
    QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";

};

TEST_F(GeometryFunctionsTest, XYZtoVector) {
    EXPECT_TRUE( corner_.Equals(WellIndexCalculator::GeometryFunctions::qvec_to_XYZ(vector_point_)) );
}

TEST_F(GeometryFunctionsTest, XYZtoVectortoXYZ) {
    EXPECT_TRUE( corner_.Equals(WellIndexCalculator::GeometryFunctions::qvec_to_XYZ( WellIndexCalculator::GeometryFunctions::XYZ_to_qvec(corner_)  )) );
}

}
