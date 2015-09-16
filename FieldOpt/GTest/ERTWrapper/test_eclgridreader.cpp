#include <gtest/gtest.h>
#include <QVector3D>
#include <iostream>
#include "eclgridreader.h"

using namespace ERTWrapper::ECLGrid;

namespace {

class ECLGridReaderTest : public ::testing::Test {
protected:
    ECLGridReaderTest() {
        ecl_grid_reader_ = new ECLGridReader();
    }

    virtual ~ECLGridReaderTest() {
        delete ecl_grid_reader_;
    }

    virtual void SetUp() {
        ecl_grid_reader_->ReadEclGrid(file_name_);
    }

    virtual void TearDown() {
        // Called immedeately after each test (right before the destructor)
    }

    ECLGridReader* ecl_grid_reader_;
    QString file_name_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";

    // Objects declared here can be used by all tests in this test case.

    void PrintCell(ECLGridReader::Cell* cell) {
        std::cout << "Cell " << cell->global_index << std::endl;
        std::cout << "\tCenter: (" << cell->center->x() << ", " << cell->center->y() << ", " << cell->center->z() << ")" << std::endl;
        std::cout << "\tCorners: " << std::endl;
        for (int i = 0; i < 8; ++i) {
            std::cout << "\t\t(" << cell->corners->at(i)->x() << ", " << cell->corners->at(i)->y() << ", " << cell->corners->at(i)->z() << ")" << std::endl;
        }
    }
};

TEST_F(ECLGridReaderTest, ReadGrid) {
    EXPECT_EQ(1620, ecl_grid_reader_->ActiveCells());
}

TEST_F(ECLGridReaderTest, ConvertIJKToGlobalIndex) {
    int global_index1 = ecl_grid_reader_->ConvertIJKToGlobalIndex(QVector3D(0, 0, 0));
    int global_index2 = ecl_grid_reader_->ConvertIJKToGlobalIndex(QVector3D(1, 0, 0));
    EXPECT_EQ(global_index1, 0);
    EXPECT_EQ(global_index2, 1);
}

TEST_F(ECLGridReaderTest, ConvertGlobalIndexToIJK) {
    QVector3D ijk1 = ecl_grid_reader_->ConvertGlobalIndexToIJK(0);
    QVector3D ijk2 = ecl_grid_reader_->ConvertGlobalIndexToIJK(1);
    EXPECT_EQ(ijk1.x(), 0); EXPECT_EQ(ijk1.y(), 0); EXPECT_EQ(ijk1.z(), 0);
    EXPECT_EQ(ijk2.x(), 1); EXPECT_EQ(ijk2.y(), 0); EXPECT_EQ(ijk2.z(), 0);
}

TEST_F(ECLGridReaderTest, CheckDimensions) {
    ECLGridReader::Dims dims = ecl_grid_reader_->Dimensions();
    EXPECT_EQ(20, dims.nx);
    EXPECT_EQ(9, dims.ny);
    EXPECT_EQ(9, dims.nz);
}

TEST_F(ECLGridReaderTest, GetCell) {
    ECLGridReader::Cell cell = ecl_grid_reader_->GetGridCell(1);
    //PrintCell(&cell);
    EXPECT_EQ(1, cell.global_index);
    EXPECT_EQ(8, cell.corners->size());
    EXPECT_EQ(cell.corners->at(0)->x(), 100);
    EXPECT_EQ(cell.corners->at(7)->z(), 7050);
    EXPECT_EQ(cell.center->x(), 150);
}

}
