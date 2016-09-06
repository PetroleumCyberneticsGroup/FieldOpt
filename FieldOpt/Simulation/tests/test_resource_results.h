#ifndef FIELDOPT_TEST_RESOURCE_RESULTS_H
#define FIELDOPT_TEST_RESOURCE_RESULTS_H

#include "Utilities/tests/test_resource_example_file_paths.hpp"
#include "Simulation/results/eclresults.h"
#include "Simulation/results/adgprsresults.h"

namespace TestResources {
    class TestResourceResults {

    protected:
        TestResourceResults() {
            results_ecl_horzwell_ = new Simulation::Results::ECLResults();
            results_ecl_horzwell_->ReadResults(ExampleFilePaths::ecl_base_horzwell);

            results_adgprs_5spot_ = new Simulation::Results::AdgprsResults();
            results_adgprs_5spot_->ReadResults(ExampleFilePaths::gprs_smry_hdf5_5spot_);

//            results_flow_5spot_ = new Simulation::Results::ECLResults();
//            results_flow_5spot_->ReadResults(ExampleFilePaths::flow_base_5spot_);
        }

        virtual ~TestResourceResults() {
            results_ecl_horzwell_->DumpResults();
            results_adgprs_5spot_->DumpResults();
//            results_flow_5spot_->DumpResults();
        }

    protected:
        Simulation::Results::ECLResults *results_ecl_horzwell_;
        Simulation::Results::AdgprsResults *results_adgprs_5spot_;
//        Simulation::Results::ECLResults *results_flow_5spot_;
    };
}

#endif //FIELDOPT_TEST_RESOURCE_RESULTS_H
