#include <gtest/gtest.h>
#include "constraints/reservoir_boundary.h"
#include "Optimization/tests/test_resource_cases.h"
#include "Reservoir/tests/test_resource_grids.h"
#include "Utilities/tests/test_resource_settings.h"

namespace {

    class ReservoirBoundaryTest : public ::testing::Test,
                                  public TestResources::TestResourceCases,
                                  public TestResources::TestResourceGrids,
                                  public TestResources::TestResourceSettings {

    public:
        ReservoirBoundaryTest() { }
        virtual ~ReservoirBoundaryTest() { }
        virtual void TearDown() { }
        virtual void SetUp() { }
    };

    TEST_F(ReservoirBoundaryTest, Initialization) {
        auto test_boundary = Optimization::Constraints::ReservoirBoundary(
            constraint_settings_reservoir_boundary_,
            variable_property_container_, grid_5spot_);
        EXPECT_FALSE(test_boundary.CaseSatisfiesConstraint(test_case_5_));
        test_boundary.SnapCaseToConstraints(test_case_5_);
        EXPECT_TRUE(test_boundary.CaseSatisfiesConstraint(test_case_5_));
    }

    // std::cout << "i=" << i << " j=" << j << " k=" << k << std::endl;
    // std::cout << grid_->GetCell(i, j, k).global_index() << std::endl;

   TEST_F(ReservoirBoundaryTest, CheckListBoxEdgeCells) {
        auto test_boundary = Optimization::Constraints::ReservoirBoundary(
           constraint_settings_reservoir_boundary_,
           variable_property_container_, grid_5spot_);

//        constraint_settings_reservoir_boundary_.
//        std::cout << "well: " << constraint_settings_reservoir_boundary_.well.toStdString();

        auto box_edge_cells_ = test_boundary.returnListOfBoxEdgeCellIndices();
//        auto box_edge_cells_ = test_boundary.index_list_edge_;

        // Debug: verbose level = 4
        if (settings_full_->verbosity()>3){
            std::cout << "length (test_reservoir_boundary.cpp):" 
            << box_edge_cells_.length() << std::endl;
        }

//        for( int i=0; i < box_edge_cells_.length(); ++i )
//        {
//           std::cout << box_edge_cells_[i] << std::endl;
//        }
        EXPECT_TRUE(box_edge_cells_.length()>0);
   }
}
