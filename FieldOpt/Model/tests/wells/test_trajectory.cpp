#include <gtest/gtest.h>
#include "Reservoir/tests/test_resource_grids.h"
#include "Model/tests/test_resource_model.h"
#include "Model/wells/well_exceptions.h"
#include "Settings/tests/test_resource_example_file_paths.hpp"

using namespace Model::Wells;

namespace {

class TrajectoryTest : public ::testing::Test,
                       public TestResources::TestResourceModel,
                       public TestResources::TestResourceGrids
{

 protected:
  TrajectoryTest() {
      prod_well_trajectory_ = model_->wells()->first()->trajectory();
  }
  virtual ~TrajectoryTest() {}
  virtual void SetUp() {}
  ::Model::Wells::Wellbore::Trajectory *prod_well_trajectory_;
};

TEST_F(TrajectoryTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(TrajectoryTest, GetWellBlock) {
    EXPECT_NO_THROW(prod_well_trajectory_->GetWellBlock(1, 4, 1));
    EXPECT_THROW(prod_well_trajectory_->GetWellBlock(9, 9, 9), WellBlockNotFoundException);

    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(1,4,1)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(1,4,1)->j());
    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(1,4,1)->k());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(1,4,1)->HasCompletion());

    EXPECT_EQ(2, prod_well_trajectory_->GetWellBlock(2,4,1)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(2,4,1)->j());
    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(2,4,1)->k());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(2,4,1)->HasCompletion());

    EXPECT_EQ(3, prod_well_trajectory_->GetWellBlock(3,4,1)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(3,4,1)->j());
    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(3,4,1)->k());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(3,4,1)->HasCompletion());

    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(4,4,1)->i());
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlock(4,4,1)->j());
    EXPECT_EQ(1, prod_well_trajectory_->GetWellBlock(4,4,1)->k());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(4,4,1)->HasCompletion());
}

TEST_F(TrajectoryTest, AllWellBlocks) {
    EXPECT_EQ(4, prod_well_trajectory_->GetWellBlocks()->size());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(0)->HasCompletion());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlocks()->at(1)->HasCompletion());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(2)->HasCompletion());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(3)->HasCompletion());
}

TEST_F(TrajectoryTest, Completions) {
    EXPECT_EQ(::Model::Wells::Wellbore::Completions::Completion::CompletionType::Perforation,
              prod_well_trajectory_->GetWellBlock(1,4,1)->GetCompletion()->type());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(1,4,1)->HasPerforation());
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlock(2,4,1)->HasPerforation());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(3,4,1)->HasPerforation());
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlock(4,4,1)->HasPerforation());
    EXPECT_FLOAT_EQ(1.0, prod_well_trajectory_->GetWellBlock(1,4,1)->GetPerforation()->transmissibility_factor());
    EXPECT_FLOAT_EQ(1.0, prod_well_trajectory_->GetWellBlock(3,4,1)->GetPerforation()->transmissibility_factor());

    EXPECT_THROW(prod_well_trajectory_->GetWellBlock(2,4,1)->GetPerforation(), PerforationNotDefinedForWellBlockException);
}

TEST_F(TrajectoryTest, VariableContainerConsistencyAfterCreation) {
    // There should be three integer variables (i,j,k) for each of the four well block
    EXPECT_EQ(9, model_->variables()->DiscreteVariableSize());
}


TEST_F(TrajectoryTest, WellBlockPerforationConsistency) {
    // The first well block should not have a completion
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(0)->HasCompletion());

    // The second well block should NOT have a completion
    EXPECT_FALSE(prod_well_trajectory_->GetWellBlocks()->at(1)->HasCompletion());

    // The third well block should have a completion
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(2)->HasCompletion());

    // The fourth well block should have a completion
    EXPECT_TRUE(prod_well_trajectory_->GetWellBlocks()->at(3)->HasCompletion());
}

TEST_F(TrajectoryTest, MultisplineWell) {
    Paths paths;
    paths.SetPath(Paths::GRID_FILE, TestResources::ExampleFilePaths::grid_5spot_.toStdString());
    auto settings = Settings::Model(TestResources::TestResourceModelSettingSnippets::model_adtl_pts(), paths);
    auto wsettings = settings.wells()[0];
    auto varcont = new Model::Properties::VariablePropertyContainer();
    auto well = Model::Wells::Wellbore::WellSpline(wsettings, varcont, TestResources::TestResourceGrids::grid_5spot_);
    auto well_blocks = well.GetWellBlocks();

    EXPECT_EQ(well_blocks->size(), 26);
    EXPECT_EQ(well_blocks->first()->i(), 13);
    EXPECT_EQ(well_blocks->first()->j(), 38);
    EXPECT_EQ(well_blocks->last()->i(), 38);
    EXPECT_EQ(well_blocks->last()->j(), 38);
}


}

