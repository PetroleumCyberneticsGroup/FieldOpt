#include <gtest/gtest.h>
#include "Model/tests/test_resource_model.h"
#include "Simulation/simulator_interfaces/flowsimulator.h"

namespace {

    class FlowDriverFileWriterTest : public ::testing::Test, TestResources::TestResourceModel {
    protected:
        FlowDriverFileWriterTest() {
//            model_ = new Model::Model(*settings_flow_5spot_->model());
//            settings_flow_5spot_->simulator()->set_driver_file_path("/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.DATA");
//            settings_flow_5spot_->model()->set_reservoir_grid_path("/home/einar/Documents/GitHub/PCG/FieldOpt/examples/Flow/5spot/5SPOT.EGRID");
//            simulator_ = new Simulation::SimulatorInterfaces::FlowSimulator(settings_flow_5spot_, model_);
        }
        virtual ~FlowDriverFileWriterTest() {}
        virtual void SetUp() {}
        Simulation::SimulatorInterfaces::FlowSimulator *simulator_;
    };

    TEST_F(FlowDriverFileWriterTest, Initialization) {
    }

    TEST_F(FlowDriverFileWriterTest, WriteDriverFile) {
//        simulator_->Evaluate();
    }

}



