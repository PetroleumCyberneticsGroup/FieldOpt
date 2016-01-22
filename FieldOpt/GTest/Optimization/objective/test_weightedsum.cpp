/******************************************************************************
 *
 *
 *
 * Created: 14.10.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include <gtest/gtest.h>
#include <QString>
#include "Simulation/results/results.h"
#include "Simulation/results/eclresults.h"
#include "Optimization/objective/weightedsum.h"
#include "Utilities/settings/settings.h"
#include "Utilities/settings/optimizer.h"


using namespace Optimization::Objective;
using namespace Simulation::Results;
using namespace Utilities::Settings;

namespace {

class WeightedSumTest : public ::testing::Test {
protected:
    WeightedSumTest()
        : settings_(driver_file_path_, output_directory_)
    {
        results_ = new ECLResults();
    }

    virtual ~WeightedSumTest() {

    }

    virtual void SetUp() {
        results_->ReadResults(file_path_);
    }

    virtual void TearDown() {
        results_->DumpResults();
    }

    Results *results_;
    QString file_path_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL";
    QString driver_file_path_ = "../../FieldOpt/GTest/Utilities/driver/driver.json";
    QString output_directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
    Settings settings_;
};

TEST_F(WeightedSumTest, Value) {
    auto *obj = new WeightedSum(settings_.optimizer(), results_);
    float wwpt = results_->GetValue(Results::Property::CumulativeWellWaterProduction, "PROD", 10);
    float fopt = results_->GetValue(Results::Property::CumulativeOilProduction);
    EXPECT_FLOAT_EQ(80.0, results_->GetValue(Results::Property::Time, 10));
    EXPECT_FLOAT_EQ(wwpt, 420.28015);
    EXPECT_FLOAT_EQ(fopt, 187866.44);
    EXPECT_FLOAT_EQ(fopt - 0.2*wwpt, obj->value());
}

}
