#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/driver_file_writers/ix_driver_file_writer.h"
#include <Settings/tests/test_resource_example_file_paths.hpp>
#include <simulator_interfaces/simulator.h>
#include "Utilities/stringhelpers.hpp"


using namespace Simulation;
using namespace TestResources;
namespace {

class IXSimulatorTest : public testing::Test, TestResourceModel {
protected:
    IXSimulatorTest() {
        Paths paths;
        paths.SetPath(Paths::DRIVER_FILE, ExampleFilePaths::norne_driver_example_);
        paths.SetPath(Paths::OUTPUT_DIR, ExampleFilePaths::norne_test_output_);
        paths.SetPath(Paths::GRID_FILE, ExampleFilePaths::norne_grid_);
        paths.SetPath(Paths::BUILD_DIR, ExampleFilePaths::bin_dir_);
        paths.SetPath(Paths::SIM_DRIVER_FILE, ExampleFilePaths::norne_deck_);
        settings_norne_full_ = new Settings::Settings(paths);
        settings_norne_optimizer_ = settings_norne_full_->optimizer();
        settings_norne_simulator_ = settings_norne_full_->simulator();
        settings_norne_model_ = settings_norne_full_->model();
        norne_model_ = new Model::Model(*settings_norne_full_, logger_norne_);
//        simulator_ = new IXSimulator(settings_norne_full_, norne_model_);

    }
  Settings::Settings *settings_norne_full_;
  Settings::Optimizer *settings_norne_optimizer_;
  Settings::Simulator *settings_norne_simulator_;
  Settings::Model *settings_norne_model_;
  Model::Model *norne_model_;
};

TEST_F(IXSimulatorTest, DriverFileWriter) {
        Simulation::IXDriverFileWriter dfw = Simulation::IXDriverFileWriter(norne_model_);
        dfw.WriteDriverFile("");
}

TEST_F(IXSimulatorTest, Evaluate) {
//    simulator_->Evaluate();
}
//
//TEST_F(IXSimulatorTest, CleanUp) {
//    //simulator_->CleanUp();
//}

}

