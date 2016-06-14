#include <gtest/gtest.h>
#include <QList>
#include <QVector3D>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"
#include "Utilities/file_handling/filehandling.h"

using namespace Reservoir::Grid;

namespace {

class DomainBoundaryTest : public ::testing::Test {
protected:
    DomainBoundaryTest() {
        grid_ = new ECLGrid(file_path_);
    }

    virtual ~DomainBoundaryTest() {
        delete grid_;
    }

    virtual void SetUp() {
    }

    virtual void TearDown() { }


    Grid *grid_;
    QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
};

TEST_F(DomainBoundaryTest, cell_boundary_constraint_test){
    Reservoir::Grid::Cell cell = grid_->GetCell(0);
    Eigen::Vector3d point_0 = Eigen::Vector3d(45,10,1724);
    Eigen::Vector3d point_1 = Eigen::Vector3d(25,25,1740);
    Eigen::Vector3d point_0_real = Eigen::Vector3d(24,10,1724);
    Eigen::Vector3d point_1_real = Eigen::Vector3d(24,24,1724);

    Eigen::Vector3d point_0_proj = WellIndexCalculator::GeometryFunctions::point_to_cell_shortest(cell,point_0);
    Eigen::Vector3d point_1_proj = WellIndexCalculator::GeometryFunctions::point_to_cell_shortest(cell,point_1);

    EXPECT_TRUE(point_0_proj==point_0_real);
    EXPECT_TRUE(point_1_proj==point_1_real);
}

TEST_F(DomainBoundaryTest, two_cells_boundary_constraint_test){
    Reservoir::Grid::Cell cell_0 = grid_->GetCell(0);
    Reservoir::Grid::Cell cell_1 = grid_->GetCell(3599);
    QList<Reservoir::Grid::Cell> cells;
    cells.append(cell_0);
    cells.append(cell_1);

    Eigen::Vector3d point_0 = Eigen::Vector3d(500,500,1724);
    Eigen::Vector3d point_0_real = Eigen::Vector3d(24,24,1724);

    Eigen::Vector3d point_0_proj = WellIndexCalculator::WellConstraintProjections::well_domain_constraint(point_0, cells);
    //Eigen::Vector3d point_1_proj = WellIndexCalculator::GeometryFunctions::point_to_cell_shortest(cell,point_1);



    std::cout << "starting point = " << std::endl << point_0 << std::endl;
    std::cout << point_0_proj << std::endl;
    //std::cout << point_0_real << std::endl;

    EXPECT_TRUE(point_0_proj==point_0_real);
}


    TEST_F(DomainBoundaryTest, point_to_line_shortest_test) {
        Reservoir::Grid::Cell cell = grid_->GetCell(0);
        Eigen::Vector3d point_0 = Eigen::Vector3d(-1,-1,1702);
        Eigen::Vector3d point_0_real = Eigen::Vector3d(0,0,1702);

        Eigen::Vector3d point_0_proj = WellIndexCalculator::GeometryFunctions::point_to_cell_shortest(cell,point_0);

        std::cout << point_0_real << std::endl;
        std::cout << point_0_proj << std::endl;
        EXPECT_TRUE(point_0_proj==point_0_real);
    }
}
