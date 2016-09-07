#include <gtest/gtest.h>
#include <QList>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/well_index_calculation/wellindexcalculator.h"
#include "Utilities/filehandling.hpp"

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;

namespace {

    class IntersectedCellsTest : public ::testing::Test {
    protected:
        IntersectedCellsTest() {
            grid_ = new ECLGrid(file_path_);
            wic_ = WellIndexCalculator(grid_);
        }

        virtual ~IntersectedCellsTest() {
            delete grid_;
        }

        virtual void SetUp() {
        }

        virtual void TearDown() { }

        Grid *grid_;
        QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
        WellIndexCalculator wic_;
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

        wic_.ComputeWellBlocks(start_point, end_point, 0.190);
        Eigen::Vector3d calc_exit_point = wic_.find_exit_point(cell_1,start_point,end_point,start_point);

        if ((calc_exit_point - start_point).dot(end_point - start_point) <= 0) {
            std::cout << "exit point wrong direction, try other direction"<<std::endl;
            calc_exit_point = wic_.find_exit_point(cell_1,start_point,end_point,calc_exit_point);
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
        QList<IntersectedCell> cells;

        wic_.ComputeWellBlocks(start_point, end_point, 0.190);

        cells  = wic_.cells_intersected();

        std::cout << "number of cells intersected = " << cells.length() << std::endl;
        for( int ii = 0; ii<cells.length(); ii++){
            std::cout << "cell intersection number " << ii+1 << " with index number " << cells[ii].global_index() << std::endl;
            std::cout << "line enters in point " << cells[ii].entry_point().x() << "," << cells[ii].entry_point().y() << "," << cells[ii].entry_point().z() << std::endl;
        }
    }

    TEST_F(IntersectedCellsTest, point_inside_cell_test) {

        // Load grid and chose first cell (cell 1,1,1)
        auto cell_0 = grid_->GetCell(0);
        auto cell_1 = grid_->GetCell(1);
        auto cell_60 = grid_->GetCell(60);

        Eigen::Vector3d point_0 = Eigen::Vector3d(0,0,1700);
        Eigen::Vector3d point_1 = Eigen::Vector3d(12,12,1712);
        Eigen::Vector3d point_2 = Eigen::Vector3d(24,12,1712);

        EXPECT_TRUE(cell_0.EnvelopsPoint(point_0));
        EXPECT_FALSE(cell_1.EnvelopsPoint(point_0));
        EXPECT_TRUE(cell_0.EnvelopsPoint(point_1));
        EXPECT_FALSE(cell_1.EnvelopsPoint(point_1));
        EXPECT_FALSE(cell_60.EnvelopsPoint(point_0));
        EXPECT_TRUE(cell_1.EnvelopsPoint(point_2));
    }

}
