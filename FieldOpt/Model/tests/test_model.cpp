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
        EXPECT_GE(model_->variables()->DiscreteVariableSize(), 9);
        EXPECT_GE(model_->variables()->ContinousVariableSize(), 5);
    }

    TEST_F(ModelTest, Variables) {
        // As of 2015.11.10, the variables are:
        // 3 Continous variables (bhp at three time steps for the producer)
        QList<Model::Properties::ContinousProperty *> prod_cont_variables = model_->variables()->GetWellControlVariables("PROD");
        EXPECT_EQ(prod_cont_variables.size(), 3);
        EXPECT_STREQ("BHP#PROD#0", prod_cont_variables[0]->name().toLatin1().constData());
        EXPECT_STREQ("BHP#PROD#50", prod_cont_variables[1]->name().toLatin1().constData());
        EXPECT_STREQ("BHP#PROD#365", prod_cont_variables[2]->name().toLatin1().constData());
        EXPECT_EQ(3, model_->wells()->at(0)->controls()->size());

        EXPECT_EQ(9, model_->variables()->GetDiscreteVariableNamesAndIdsMatchingSubstring("WellBlock#PROD").size());
//        EXPECT_EQ(12, model_->variables()->GetDiscreteVariableIdsWithName("PROD-WELLBLOCKS-ALL#0#i").size()); // Three variables pr. block (i,j,k)
        foreach (int value, model_->variables()->GetDiscreteVariableValues().values()) {
            EXPECT_GE(value, 0);
        }
    }

    TEST_F(ModelTest, ApplyCase) {
        Optimization::Case *c = new ::Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                                         model_->variables()->GetDiscreteVariableValues(),
                                                         model_->variables()->GetContinousVariableValues());

        // Set all continous variables for the PROD well to 1. Should affect BHP and transmissibilty.
        auto producer_vars = model_->variables()->GetContinousVariableNamesAndIdsMatchingSubstring("PROD");
        foreach (auto var, producer_vars) {
                std::cout << "Setting value for " << var.second.toStdString() << std::endl;
            c->set_real_variable_value(var.first, 1.0);
        }

        // Set all integer coordinates to 1 (should affect positions for all well blocks)
        auto producer_disc_vars = model_->variables()->GetDiscreteVariableNamesAndIdsMatchingSubstring("PROD");
        foreach (auto var, producer_disc_vars) {
                std::cout << "Setting value for " << var.second.toStdString() << std::endl;
                c->set_integer_variable_value(var.first, 1);
        }

        model_->ApplyCase(c);

        foreach (Model::Wells::Control *control, *model_->wells()->first()->controls()) {
            EXPECT_FLOAT_EQ(1.0, control->bhp());
        }

        for (int i = 0; i < model_->wells()->first()->trajectory()->GetWellBlocks()->size(); ++i) {
            auto wb = model_->wells()->first()->trajectory()->GetWellBlocks()->at(i);
            if (i == 1) { //!< WB 1 is not variable, and should thus not have been changed
                EXPECT_EQ(2, wb->i());
                EXPECT_EQ(4, wb->j());
                EXPECT_EQ(1, wb->k());
            }
            else {
                EXPECT_EQ(1, wb->i());
                EXPECT_EQ(1, wb->j());
                EXPECT_EQ(1, wb->k());
            }
        }
    }

}
