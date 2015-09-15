#include <gtest/gtest.h>
#include "eclgridreader.h"

using namespace ERTWrapper::ECLGrid;

namespace {

class ECLGridReaderTest : public ::testing::Test {
protected:
    ECLGridReaderTest() {
        ecl_grid_reader = new ECLGridReader();
    }

    virtual ~ECLGridReaderTest() {
        delete ecl_grid_reader;
    }

    virtual void SetUp() {
        ecl_grid_reader->ReadEclGrid(file_name);
    }

    virtual void TearDown() {
        // Called immedeately after each test (right before the destructor)
    }

    ECLGridReader* ecl_grid_reader;
    QString file_name = "../../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID";

    // Objects declared here can be used by all tests in this test case.
};

TEST_F(ECLGridReaderTest, ReadGrid) {
    EXPECT_EQ(1620, ecl_grid_reader->ActiveCells());
}

TEST_F(ECLGridReaderTest, CheckDimensions) {
    ECLGridReader::dimensions dims = ecl_grid_reader->Dimensions();
    EXPECT_EQ(20, dims.x);
    EXPECT_EQ(9, dims.y);
    EXPECT_EQ(9, dims.z);
}

}
