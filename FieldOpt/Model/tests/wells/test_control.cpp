#include <gtest/gtest.h>
#include <tests/test_resource_model.h>
#include "Model/wells/control.h"
#include "Model/tests/test_resource_model.h"

using namespace Model::Wells;

namespace {

class ControlTest : public testing::Test, public TestResources::TestResourceModel {
protected:
    ControlTest() {
        well_ = settings_model_->wells().first();
        inj_well_ = settings_model_->wells().last();
        entry_ = well_.controls.first();
        inj_entry_ = inj_well_.controls.first();

        for (int i = 0; i < settings_model_->wells().size(); ++i) {
            for (int j = 0; j < settings_model_->wells()[i].controls.size(); ++j) {
                all_controls_.append(new Control(settings_model_->wells()[i].controls[j],
                                                 settings_model_->wells()[i],
                                                 model_->variables(),
                                                 model_->variable_handler()));
            }
        }
    }
    virtual ~ControlTest() {}
    virtual void SetUp() {}

    ::Utilities::Settings::Model::Well well_;
    ::Utilities::Settings::Model::Well inj_well_;
    ::Utilities::Settings::Model::ControlEntry entry_;
    ::Utilities::Settings::Model::ControlEntry inj_entry_;
    QList<Control *> all_controls_;
};

TEST_F(ControlTest, ProducerControl) {

    EXPECT_STREQ("PROD", well_.name.toLatin1().constData());
    EXPECT_EQ(0, entry_.time_step);
    EXPECT_TRUE(all_controls_.first()->open());
    EXPECT_EQ(::Utilities::Settings::Model::ControlMode::BHPControl, all_controls_.first()->mode());
    EXPECT_FLOAT_EQ(2000, all_controls_.first()->bhp());
}

TEST_F(ControlTest, InjectorControl) {
    EXPECT_STREQ("INJ", inj_well_.name.toLatin1().constData());
    EXPECT_EQ(0, inj_entry_.time_step);
    EXPECT_EQ(0, all_controls_.last()->time_step());
    EXPECT_TRUE(all_controls_.last()->open());
    EXPECT_EQ(::Utilities::Settings::Model::ControlMode::RateControl, all_controls_.last()->mode());
    EXPECT_FLOAT_EQ(1200, all_controls_.last()->rate());
}

TEST_F(ControlTest, VariableContainerConsistencyAfterCreation) {
    EXPECT_EQ(0, model_->variables()->BinaryVariableSize());
    EXPECT_EQ(12, model_->variables()->DiscreteVariableSize());
    EXPECT_EQ(14, model_->variables()->ContinousVariableSize());
}

}
