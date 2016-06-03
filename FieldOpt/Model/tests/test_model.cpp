#include <gtest/gtest.h>
#include "test_resource_model.h"

namespace {

class ModelTest : public ::testing::Test,  public TestResources::TestResourceModel {
protected:
    ModelTest() { }

};

TEST_F(ModelTest, Constructor) {
    EXPECT_TRUE(true);
}

TEST_F(ModelTest, ChildObjects) {
    EXPECT_NO_THROW(model_->reservoir());
    EXPECT_NO_THROW(model_->wells());
    EXPECT_NO_THROW(model_->variables());
    EXPECT_GE(model_->wells()->size(), 2);
    EXPECT_GE(model_->variables()->BinaryVariableSize(), 0);
    EXPECT_GE(model_->variables()->DiscreteVariableSize(), 12);
    EXPECT_GE(model_->variables()->ContinousVariableSize(), 5);
}

TEST_F(ModelTest, Variables) {
    // As of 2015.11.10, the variables are:
    // 3 Continous variables (bhp at three time steps for the producer)
    EXPECT_TRUE(model_->variable_handler()->GetControl("PROD", 0)->bhp());
    EXPECT_TRUE(model_->variable_handler()->GetControl("PROD", 50)->bhp());
    EXPECT_TRUE(model_->variable_handler()->GetControl("PROD", 365)->bhp());
    EXPECT_STREQ("PROD-BHP-1", model_->variable_handler()->GetControl("PROD", 0)->variable_name().toLatin1().constData());
    EXPECT_STREQ("PROD-BHP-1", model_->variable_handler()->GetControl("PROD", 50)->variable_name().toLatin1().constData());
    EXPECT_STREQ("PROD-BHP-1", model_->variable_handler()->GetControl("PROD", 365)->variable_name().toLatin1().constData());
    EXPECT_EQ(3, model_->variables()->GetContinousVariableIdsWithName("PROD-BHP-1").size());
    EXPECT_EQ(3, model_->wells()->at(0)->controls()->size());


    // 12 Discrete variables for the positions for the producer's four well blocks
    EXPECT_TRUE(model_->variable_handler()->GetWellBlock(0)->position());
    EXPECT_TRUE(model_->variable_handler()->GetWellBlock(1)->position());
    EXPECT_TRUE(model_->variable_handler()->GetWellBlock(2)->position());
    EXPECT_TRUE(model_->variable_handler()->GetWellBlock(3)->position());
    EXPECT_STREQ("PROD-WELLBLOCKS-ALL", model_->variable_handler()->GetWellBlock(0)->variable_name().toLatin1().constData());
    EXPECT_STREQ("PROD-WELLBLOCKS-ALL", model_->variable_handler()->GetWellBlock(1)->variable_name().toLatin1().constData());
    EXPECT_STREQ("PROD-WELLBLOCKS-ALL", model_->variable_handler()->GetWellBlock(2)->variable_name().toLatin1().constData());
    EXPECT_STREQ("PROD-WELLBLOCKS-ALL", model_->variable_handler()->GetWellBlock(3)->variable_name().toLatin1().constData());
    EXPECT_EQ(12, model_->variables()->GetDiscreteVariableIdsWithName("PROD-WELLBLOCKS-ALL#0#i").size()); // Three variables pr. block (i,j,k)
    foreach (int value, model_->variables()->GetDiscreteVariableValues().values()) {
        EXPECT_GE(value, 0);
    }
}

TEST_F(ModelTest, ApplyCase) {
    Optimization::Case *c = new ::Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                                     model_->variables()->GetDiscreteVariableValues(),
                                                     model_->variables()->GetContinousVariableValues());

    // Set all continous variables to 1.0 (should affect BHP and Transmissibility in the model)
    foreach (QUuid key, c->real_variables().keys()) {
        std::cout << model_->variables()->GetContinousVariableIdsWithName("INJ-SplinePoints-1").size() << std::endl;
        if (model_->variables()->GetContinousVariableIdsWithName("INJ-SplinePoints-1").contains(key))
            continue;
        c->set_real_variable_value(key, 1.0);
    }

    // Set all integer coordinates to 1 (should affect positions for all well blocks)
    foreach (QUuid key, c->integer_variables().keys()) {
        c->set_integer_variable_value(key, 1);
    }

    model_->ApplyCase(c);

    foreach (Model::Wells::Control *control, *model_->wells()->first()->controls()) {
        EXPECT_FLOAT_EQ(1.0, control->bhp());
    }

    foreach (Model::Wells::Wellbore::WellBlock *wb, *model_->wells()->first()->trajectory()->GetWellBlocks()) {
        EXPECT_EQ(1, wb->i());
        EXPECT_EQ(1, wb->j());
        EXPECT_EQ(1, wb->k());
    }
}

}
