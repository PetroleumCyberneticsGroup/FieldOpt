#include <gtest/gtest.h>
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"
#include "Model/tests/test_resource_model.h"

using namespace Model::Wells::Wellbore;

namespace {

class PerforationTest : public testing::Test, public TestResources::TestResourceModel {
protected:
    PerforationTest() {
        prod_well_ = settings_full_->model()->wells().first();
    }
    virtual ~PerforationTest() {}
    virtual void SetUp() {
        prod_perforations_ = QList<Completions::Perforation *>();
        for (int i = 0; i < prod_well_.well_blocks.size(); ++i) {
            if (prod_well_.well_blocks[i].has_completion) {
                prod_perforations_.append(new Completions::Perforation(prod_well_.well_blocks[i].completion,
                                                                       model_->variables()));
            }
        }
    }

    ::Utilities::Settings::Model::Well prod_well_;
    QList<Completions::Perforation *> prod_perforations_;
};

TEST_F(PerforationTest, Constructor) {
    EXPECT_EQ(3, prod_perforations_.size());
}

TEST_F(PerforationTest, Values) {
    EXPECT_FLOAT_EQ(1.0, prod_perforations_[0]->transmissibility_factor());
    EXPECT_FLOAT_EQ(1.0, prod_perforations_[1]->transmissibility_factor());
}

}
