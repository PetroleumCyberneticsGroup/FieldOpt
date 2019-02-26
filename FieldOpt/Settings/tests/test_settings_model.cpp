#include <gtest/gtest.h>
#include <QString>

#include "Settings/tests/test_resource_settings.hpp"
#include "Settings/tests/test_resource_multispline_wells_settings.hpp"

using namespace Settings;

namespace {

class ModelSettingsTest : public ::testing::Test,
                          public TestResources::TestResourceSettings {
 protected:
  ModelSettingsTest()
  { }
  virtual ~ModelSettingsTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(ModelSettingsTest, ControlTimes) {
    EXPECT_EQ(4, settings_model_->control_times().size());
    EXPECT_EQ(365, settings_model_->control_times().last());
}

TEST_F(ModelSettingsTest, ProducerWell) {
    Model::Well producer = settings_model_->wells().first();
    EXPECT_STREQ("PROD", producer.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Producer, producer.type);
    EXPECT_EQ(Model::WellDefinitionType::WellBlocks, producer.definition_type);
    EXPECT_EQ(Model::PreferredPhase::Oil, producer.preferred_phase);
    EXPECT_FLOAT_EQ(0.75, producer.wellbore_radius);
    EXPECT_EQ(Model::Direction::X, producer.direction);
}

TEST_F(ModelSettingsTest, ProducerCompletions) {
    Model::Well producer = settings_model_->wells().first();
    EXPECT_EQ(4, producer.well_blocks.size());
    for (int i = 0; i < producer.well_blocks.size(); ++i) {
        EXPECT_EQ(i+1, producer.well_blocks[i].i);
        EXPECT_EQ(4, producer.well_blocks[i].j);
        EXPECT_EQ(1, producer.well_blocks[i].k);
        QString expected_name = QString("WellBlock#PROD#%1").arg(i);
        EXPECT_STREQ(expected_name.toLatin1().constData(), producer.well_blocks[i].name.toLatin1().constData());
        if (i == 1) {
            EXPECT_FALSE(producer.well_blocks[i].is_variable);
            EXPECT_TRUE(producer.well_blocks[i].has_completion);
        }
        else {
            EXPECT_TRUE(producer.well_blocks[i].is_variable);
            EXPECT_TRUE(producer.well_blocks[i].has_completion);
            EXPECT_FLOAT_EQ(1.0, producer.well_blocks[i].completion.transmissibility_factor);
            EXPECT_EQ(Model::WellCompletionType::Perforation, producer.well_blocks[i].completion.type);
            QString expected_name = QString("Transmissibility#PROD#%1").arg(i);
            EXPECT_STREQ(expected_name.toLatin1().constData(), producer.well_blocks[i].completion.name.toLatin1().constData());
        }
    }
}

TEST_F(ModelSettingsTest, ProducerControls) {
    Model::Well producer = settings_model_->wells().first();
    EXPECT_EQ(3, producer.controls.length());
    for (auto control : producer.controls) {
        QString expected_name = QString("BHP#PROD#%1").arg(control.time_step);
        EXPECT_STREQ(expected_name.toLatin1().constData(), control.name.toLatin1().constData());
        EXPECT_EQ(Model::ControlMode::BHPControl, control.control_mode);
        EXPECT_FLOAT_EQ(100.0, control.bhp);
        EXPECT_LE(0, control.time_step);
        EXPECT_GE(365, control.time_step);
        EXPECT_EQ(Model::WellState::WellOpen, control.state);
        EXPECT_TRUE(control.is_variable);
    }
}

TEST_F(ModelSettingsTest, InjectorWell) {
    Model::Well injector = settings_model_->wells()[1];
    EXPECT_STREQ("INJ", injector.name.toLatin1().constData());
    EXPECT_EQ(Model::WellType::Injector, injector.type);
    EXPECT_EQ(Model::PreferredPhase::Water, injector.preferred_phase);
    EXPECT_FLOAT_EQ(0.75, injector.wellbore_radius);
    EXPECT_EQ(Model::WellDefinitionType::WellSpline, injector.definition_type);
}


TEST_F(ModelSettingsTest, InjectorSpline) {
    Model::Well injector = settings_model_->wells()[1];

    EXPECT_FLOAT_EQ(12.0, injector.spline_heel.x);
    EXPECT_FLOAT_EQ(12.0, injector.spline_heel.y);
    EXPECT_FLOAT_EQ(1712.0, injector.spline_heel.z);
    EXPECT_TRUE(injector.spline_heel.is_variable);
    EXPECT_STREQ("SplinePoint#INJ#heel", injector.spline_heel.name.toLatin1().constData());

    EXPECT_FLOAT_EQ(36.0, injector.spline_toe.x);
    EXPECT_FLOAT_EQ(12.0, injector.spline_toe.y);
    EXPECT_FLOAT_EQ(1712.0, injector.spline_toe.z);
    EXPECT_TRUE(injector.spline_toe.is_variable);
    EXPECT_STREQ("SplinePoint#INJ#toe", injector.spline_toe.name.toLatin1().constData());
}

TEST_F(ModelSettingsTest, InjectorCompletions) {
    Model::Well injector = settings_model_->wells()[1];
    // The injector should not have any completions or well blocks
    EXPECT_EQ(0, injector.well_blocks.length());
}

TEST_F(ModelSettingsTest, InjectorControls) {
    Model::Well injector = settings_model_->wells()[1];
    EXPECT_EQ(1, injector.controls.length());
    EXPECT_EQ(0, injector.controls[0].time_step);
    EXPECT_EQ(Model::InjectionType::WaterInjection, injector.controls[0].injection_type);
    EXPECT_EQ(Model::WellState::WellOpen, injector.controls[0].state);
    EXPECT_EQ(Model::ControlMode::RateControl, injector.controls[0].control_mode);
    EXPECT_FLOAT_EQ(1200, injector.controls[0].rate);
    EXPECT_FALSE(injector.controls[0].is_variable);
    EXPECT_STREQ("Rate#INJ#0", injector.controls[0].name.toLatin1().constData());
}

TEST_F(ModelSettingsTest, MultisplineWell) {

    Paths paths_;
    QJsonObject partial_deck = model_adtl_pts;
    paths_.SetPath(Paths::SIM_DRIVER_FILE, TestResources::ExampleFilePaths::norne_deck_ );
    paths_.SetPath(Paths::SIM_SCH_FILE, TestResources::ExampleFilePaths::norne_sch_);
//    QJsonObject sim_json_;
    QJsonObject mod_json_;
//    sim_json_ = partial_deck["Simulator"].toObject();
    mod_json_ = partial_deck["Model"].toObject();
//    auto sim_settings = Settings::Simulator(sim_json_, paths_);
    auto mod_settings = Settings::Model(mod_json_, paths_);
}

}
