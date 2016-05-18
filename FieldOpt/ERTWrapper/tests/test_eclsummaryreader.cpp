/******************************************************************************
 *
 * test_eclsummaryreader.cpp
 *
 * Created: 07.10.2015 2015 by einar
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
#include "ERTWrapper/eclsummaryreader.h"
#include "ERTWrapper/ertwrapper_exceptions.h"

using namespace ERTWrapper::ECLSummary;

namespace {

class ECLSummaryReaderTest : public ::testing::Test {
protected:
    ECLSummaryReaderTest() {
        ecl_summary_reader_ = new ECLSummaryReader(file_name_);
    }

    virtual ~ECLSummaryReaderTest() {
        delete ecl_summary_reader_;
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    ECLSummaryReader *ecl_summary_reader_;
    QString file_name_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL";
};

TEST_F(ECLSummaryReaderTest, ReportSteps) {
    EXPECT_EQ(0, ecl_summary_reader_->GetFirstReportStep());
    EXPECT_EQ(22, ecl_summary_reader_->GetLastReportStep());
    EXPECT_TRUE(ecl_summary_reader_->HasReportStep(10));
    EXPECT_FALSE(ecl_summary_reader_->HasReportStep(-1));
    EXPECT_FALSE(ecl_summary_reader_->HasReportStep(32));
}

TEST_F(ECLSummaryReaderTest, MiscVar) {
    EXPECT_NO_THROW(ecl_summary_reader_->GetMiscVar("TIME", 1));
    EXPECT_THROW(ecl_summary_reader_->GetMiscVar("NOVAR", 1), ERTWrapper::SummaryVariableDoesNotExistException);
    EXPECT_THROW(ecl_summary_reader_->GetMiscVar("TIME", 23), ERTWrapper::SummaryTimeStepDoesNotExistException);
    EXPECT_FLOAT_EQ(200, ecl_summary_reader_->GetMiscVar("TIME", 22));
    EXPECT_FLOAT_EQ(0, ecl_summary_reader_->GetMiscVar("TIME", 0));
}

TEST_F(ECLSummaryReaderTest, FieldVar) {
    EXPECT_NO_THROW(ecl_summary_reader_->GetFieldVar("FOPT", 0));
    EXPECT_THROW(ecl_summary_reader_->GetFieldVar("NOVAR", 1), ERTWrapper::SummaryVariableDoesNotExistException);
    EXPECT_THROW(ecl_summary_reader_->GetFieldVar("FOPT", 32), ERTWrapper::SummaryTimeStepDoesNotExistException);
    EXPECT_FLOAT_EQ(0, ecl_summary_reader_->GetFieldVar("FOPT", 0));
    EXPECT_FLOAT_EQ(187866.44, ecl_summary_reader_->GetFieldVar("FOPT", 22));
}

TEST_F(ECLSummaryReaderTest, WellVar) {
    EXPECT_NO_THROW(ecl_summary_reader_->GetWellVar("PROD", "WOPR", 1));
    EXPECT_THROW(ecl_summary_reader_->GetWellVar("NOWELL", "WOPR", 1), ERTWrapper::SummaryVariableDoesNotExistException);
    EXPECT_THROW(ecl_summary_reader_->GetWellVar("PROD", "NOVAR", 1), ERTWrapper::SummaryVariableDoesNotExistException);
    EXPECT_THROW(ecl_summary_reader_->GetWellVar("PROD", "WOPR", 32), ERTWrapper::SummaryTimeStepDoesNotExistException);
    EXPECT_FLOAT_EQ(0, ecl_summary_reader_->GetWellVar("PROD", "WOPR", 0));
    EXPECT_FLOAT_EQ(628.987, ecl_summary_reader_->GetWellVar("PROD", "WOPR", 22));
}

}
