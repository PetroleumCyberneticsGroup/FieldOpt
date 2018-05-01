#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/eclsimulator.h"
#include <Settings/tests/test_resource_example_file_paths.hpp>
#include "Utilities/stringhelpers.hpp"


using namespace Simulation::SimulatorInterfaces;
using namespace TestResources;
namespace {

class ECLSimulatorTest : public testing::Test, TestResourceModel {
protected:
    ECLSimulatorTest() {
        settings_norne_full_ = new Settings::Settings(ExampleFilePaths::norne_driver_example_,
                                                      ExampleFilePaths::norne_test_output_);
        settings_norne_optimizer_ = settings_norne_full_->optimizer();
        settings_norne_simulator_ = settings_norne_full_->simulator();
        settings_norne_model_ = settings_norne_full_->model();
        simulator_ = new ECLSimulator(settings_norne_full_, norne_model_);

        settings_norne_model_->set_reservoir_grid_path(ExampleFilePaths::norne_grid_);
        norne_model_ = new Model::Model(*settings_norne_model_, logger_norne_);
    }
    Simulator *simulator_;
  Settings::Settings *settings_norne_full_;
  Settings::Optimizer *settings_norne_optimizer_;
  Settings::Simulator *settings_norne_simulator_;
  Settings::Model *settings_norne_model_;
  Model::Model *norne_model_;
};

/*!
 * \note No more than one of these tests should be uncommented at a time
 * because running the deck parser more than once, for some reason, causes errors
 * in the OPM library. The deck parser is executed every time the ECLSimulatorTest
 * constructor is called, which is every time a test in this class is executed.
 */
TEST_F(ECLSimulatorTest, Constructor) {
    EXPECT_TRUE(true);
}

//TEST_F(ECLSimulatorTest, Evaluate) {
//    simulator_->Evaluate();
//}
//
//TEST_F(ECLSimulatorTest, CleanUp) {
//    //simulator_->CleanUp();
//}

}
