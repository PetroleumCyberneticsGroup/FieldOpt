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
#include <iostream>
#include "Simulation/results/eclresults.h"
#include "Simulation/results/results_exceptions.h"

using namespace Simulation::Results;

namespace {

class ECLResultsTest : public ::testing::Test {
protected:
    ECLResultsTest() {
        results_ = new ECLResults();
    }

    virtual ~ECLResultsTest() {

    }

    virtual void SetUp() {

    }

    virtual void TearDown() {
        results_->DumpResults();
    }

    Results *results_;
    QString file_path_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL";
};

TEST_F(ECLResultsTest, ReadSummary) {
    EXPECT_THROW(results_->ReadResults("a"), ResultFileNotFoundException);
    EXPECT_NO_THROW(results_->ReadResults(file_path_));
}

TEST_F(ECLResultsTest, DumpingAndAvailability) {
    // Make results available
    results_->ReadResults(file_path_);
    EXPECT_TRUE(results_->isAvailable());
    EXPECT_NO_THROW(results_->GetValue(ECLResults::Property::Time));

    // Make results unavailable
    results_->DumpResults();
    EXPECT_FALSE(results_->isAvailable());
    EXPECT_THROW(results_->GetValue(ECLResults::Property::Time), ResultsNotAvailableException);

    // Make results available again
    results_->ReadResults(file_path_);
    EXPECT_TRUE(results_->isAvailable());
    EXPECT_NO_THROW(results_->GetValue(ECLResults::Property::Time));
}

TEST_F(ECLResultsTest, FieldVariables) {
    results_->ReadResults(file_path_);
    EXPECT_FLOAT_EQ(187866.44, results_->GetValue(ECLResults::Property::CumulativeOilProduction));
    EXPECT_FLOAT_EQ(98328.047, results_->GetValue(ECLResults::Property::CumulativeOilProduction, 10));
    EXPECT_THROW(results_->GetValue(ECLResults::Property::CumulativeOilProduction, 30), ResultTimeIndexInvalidException);
}

TEST_F(ECLResultsTest, MiscVariables) {
    results_->ReadResults(file_path_);
    EXPECT_FLOAT_EQ(200, results_->GetValue(ECLResults::Property::Time));
    EXPECT_FLOAT_EQ(80, results_->GetValue(ECLResults::Property::Time, 10));
    EXPECT_THROW(results_->GetValue(ECLResults::Property::Time, 30), ResultTimeIndexInvalidException);
}

TEST_F(ECLResultsTest, WellVariables) {
    results_->ReadResults(file_path_);
    EXPECT_FLOAT_EQ(1116.8876, results_->GetValue(ECLResults::Property::CumulativeWellWaterProduction, "PROD"));
    EXPECT_FLOAT_EQ(420.28015, results_->GetValue(ECLResults::Property::CumulativeWellWaterProduction, "PROD", 10));
}

}
