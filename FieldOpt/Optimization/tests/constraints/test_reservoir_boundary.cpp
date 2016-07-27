#include <gtest/gtest.h>
#include "constraints/reservoir_boundary.h"
#include "Optimization/tests/test_resource_cases.h"
#include "Optimization/tests/test_resource_optimizer.h"
#include "Reservoir/tests/test_resource_grids.h"

namespace {

    class ReservoirBoundaryTest : public ::testing::Test,
                                  public TestResources::TestResourceCases,
                                  public TestResources::TestResourceGrids,
                                  public TestResources::TestResourceSettings {

    public:
        ReservoirBoundaryTest() {

            bound_settings_.type = Utilities::Settings::Optimizer::ConstraintType::ReservoirBoundary;
            bound_settings_.well = settings_optimizer_->constraints()[5].wells[1]; // TESTW
            bound_settings_.box_imin = settings_optimizer_->constraints()[5].box_imin;
            bound_settings_.box_imax = settings_optimizer_->constraints()[5].box_imax;
            bound_settings_.box_jmin = settings_optimizer_->constraints()[5].box_jmin;
            bound_settings_.box_jmax = settings_optimizer_->constraints()[5].box_jmax;
            bound_settings_.box_kmin = settings_optimizer_->constraints()[5].box_kmin;
            bound_settings_.box_kmax = settings_optimizer_->constraints()[5].box_kmax;

            boundary_constraint_ = new Optimization::Constraints::ReservoirBoundary(
                    bound_settings_, variable_property_container_, grid_5spot_);
        }

        Utilities::Settings::Optimizer::Constraint bound_settings_;
        Optimization::Constraints::ReservoirBoundary *boundary_constraint_;

        virtual ~ReservoirBoundaryTest() { }
        virtual void TearDown() { }
        virtual void SetUp() { }

    };

    TEST_F(ReservoirBoundaryTest, Initialization) {
        EXPECT_FALSE(boundary_constraint_->CaseSatisfiesConstraint(test_case_5_));
        boundary_constraint_->SnapCaseToConstraints(test_case_5_);
        EXPECT_TRUE(boundary_constraint_->CaseSatisfiesConstraint(test_case_5_));
    }

   TEST_F(ReservoirBoundaryTest, CheckListBoxEdgeCells) {

        auto box_edge_cells_ = boundary_constraint_->returnListOfBoxEdgeCellIndices();

        // Debug: verbose level = 4
        int verbosity_level_ = 0;
        if (verbosity_level_>0){
            std::cout << "length box_edge_cells_: "
            << box_edge_cells_.length() << std::endl;
        }

//        for( int i=0; i < box_edge_cells_.length(); ++i )
//        {
//           std::cout << box_edge_cells_[i] << std::endl;
//        }
        EXPECT_TRUE(box_edge_cells_.length()>0);
   }

    TEST_F(ReservoirBoundaryTest, CheckBoundConstraintSettings) {
        // std::cout << "i=" << i << " j=" << j << " k=" << k << std::endl;
        // std::cout << grid_->GetCell(i, j, k).global_index() << std::endl;

        boundary_constraint_->findCornerCells();
    }
}

