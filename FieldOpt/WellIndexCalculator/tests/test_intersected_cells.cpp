#include <gtest/gtest.h>
#include <QList>
#include <QVector3D>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "Utilities/file_handling/filehandling.h"

using namespace Reservoir::Grid;

namespace {

    class IntersectedCellsTest : public ::testing::Test {
    protected:
        IntersectedCellsTest() {
            grid_ = new ECLGrid(file_path_);
        }

        virtual ~IntersectedCellsTest() {
            delete grid_;
        }

        virtual void SetUp() {
        }

        virtual void TearDown() { }

        Grid *grid_;
        QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
    };

    TEST_F(IntersectedCellsTest, find_point_test) {
        std::cout << "find exit point test"<<std::endl;
        // Load grid and chose first cell (cell 1,1,1)
        auto cell_1 = grid_->GetCell(0);
        //auto ptr_cell_1 = &cell_1;

        Eigen::Vector3d start_point = Eigen::Vector3d(0,0,1712);
        Eigen::Vector3d end_point = Eigen::Vector3d(25,25,1712);
        Eigen::Vector3d exit_point = Eigen::Vector3d(24,24,1712);
        std::cout << "find exit point test"<<std::endl;
        Eigen::Vector3d calc_exit_point = WellIndexCalculator::GeometryFunctions::find_exit_point(cell_1,start_point,end_point,start_point);

        if ((calc_exit_point - start_point).dot(end_point - start_point) <= 0) {
            std::cout << "exit point wrong direction, try other direction"<<std::endl;
            calc_exit_point = WellIndexCalculator::GeometryFunctions::find_exit_point(cell_1,start_point,end_point,calc_exit_point);
            std::cout << "new algorith exit point = " << calc_exit_point.x() << "," << calc_exit_point.y() << "," << calc_exit_point.z() << std::endl;
        }
        std::cout << "algorith exit point = " << calc_exit_point.x() << "," << calc_exit_point.y() << "," << calc_exit_point.z() << std::endl;
        std::cout << "actual exit point = " << exit_point.x() << "," << exit_point.y() << "," << exit_point.z() << std::endl;
        double diff = (exit_point - calc_exit_point).norm();

        EXPECT_TRUE(diff<10-8);

    }

    TEST_F(IntersectedCellsTest, intersected_cell_test_cases) {

        // Load grid and chose first cell (cell 1,1,1)
        auto cell_0 = grid_->GetCell(0);
        //auto ptr_cell_1 = &cell_1;
        Eigen::Vector3d start_point = Eigen::Vector3d(0,0,1702);
        Eigen::Vector3d end_point = Eigen::Vector3d(44,84,1720);
        QPair<QList<int>,QList<Eigen::Vector3d>> pair;

        pair  = WellIndexCalculator::GeometryFunctions::cells_intersected(start_point,end_point,grid_);

        std::cout << "number of cells intersected = " << pair.first.length() << std::endl;
        std::cout << "number of intersection points = " << pair.second.length() << std::endl;
        for( int ii = 0; ii<pair.first.length(); ii++){
            std::cout << "cell intersection number " << ii+1 << " with index number " << pair.first.at(ii) << std::endl;
            std::cout << "line enters in point " << pair.second.at(ii).x() << "," << pair.second.at(ii).y() << "," << pair.second.at(ii).z() << std::endl;
        }

        EXPECT_TRUE( pair.first.length() + 1 == pair.second.length());
    }

    TEST_F(IntersectedCellsTest, point_inside_cell_test) {

        // Load grid and chose first cell (cell 1,1,1)
        auto cell_0 = grid_->GetCell(0);
        auto cell_1 = grid_->GetCell(1);
        auto cell_60 = grid_->GetCell(60);

        QVector3D point_0 = QVector3D(0,0,1700);
        QVector3D point_1 = QVector3D(12,12,1712);
        QVector3D point_2 = QVector3D(24,12,1712);

        EXPECT_TRUE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_0,point_0));
        EXPECT_FALSE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_1,point_0));
        EXPECT_TRUE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_0,point_1));
        EXPECT_FALSE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_1,point_1));
        EXPECT_FALSE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_60,point_0));
        EXPECT_TRUE(WellIndexCalculator::GeometryFunctions::is_point_inside_cell(cell_1,point_2));
    }

}
